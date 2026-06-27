#include "Arduino.h"
#include "ForecastObserver.h"
#include "WeatherData.h"
#include "WiFiClient.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "rgb_lcd.h"

// Relevant: https://docs.arduino.cc/learn/programming/memory-guide/
// Also relevant: https://docs.arduino.cc/learn/programming/reference/

ForecastObserver::ForecastObserver(float longitude, float latitude) {
  _longitude = longitude;
  _latitude = latitude;
  _weatherData = new WeatherData*[NUM_OF_FORECASTS + 1]();
  _rowToRead = 0;

  updateData();
}

ForecastObserver::~ForecastObserver() {
  for(int i = 0; i < NUM_OF_FORECASTS; i++) {
    if(_weatherData[i] != nullptr) {
      delete _weatherData[i]; // Dereference or anything?
    }
  }

  delete[] _weatherData;
  Serial.println(F("*!* ForecastObserver Destroyed *!*"));
}

void ForecastObserver::setNewCoordinates(float longitude, float latitude) {
  _longitude = longitude;
  _latitude = latitude;
}

WeatherData* ForecastObserver::getWorstWeather(int startIndex) {
  WeatherData* worstWeather;
  int highestSeverity = 0;
  int highestCategory = 0;
  for(int i = startIndex; i < NUM_OF_FORECASTS; i++) {
    WeatherData* data = _weatherData[i];
    int generalCategory = data->getCategoryValue();
    int severity = data->getSeverityValue();
    if(generalCategory > highestCategory) {
      worstWeather = data;
      highestCategory = generalCategory;
      highestSeverity = severity;
    } else if(highestCategory == generalCategory && severity > highestSeverity) {
      worstWeather = data;
      highestSeverity = severity;
    }
  }

  // If weather conditions are the same all day, nothing gets assigned. So return current.
  return worstWeather != nullptr ? worstWeather : _weatherData[0];
}

WeatherData* ForecastObserver::getBestWeather(int startIndex) {
  WeatherData* bestWeather;
  int lowestSeverity = 32;  // Arbitrary high numbers, as we want to find the lowest.
  int lowestCategory = 32;
  for(int i = startIndex; i < NUM_OF_FORECASTS; i++) {
    WeatherData* data = _weatherData[i];
    int generalCategory = data->getCategoryValue();
    int severity = data->getSeverityValue();
    if(generalCategory < lowestCategory) {
      bestWeather = data;
      lowestCategory = data->getCategoryValue();
      lowestSeverity = data->getSeverityValue();
    } else if(lowestCategory == generalCategory && severity < lowestSeverity) {
      bestWeather = data;
      lowestSeverity = severity;
    }
  }

  return bestWeather != nullptr ? bestWeather : _weatherData[0];
}
    
float ForecastObserver::getCurrentTemperature() {
  return _weatherData[0]->getTemperature();
}

// Returns true if a predetermined amount of time has passed since the last download.
bool ForecastObserver::readyToUpdate() {
  return _nextUpdate < millis();
}

// Sets the time to download new data to now.
void ForecastObserver::requestUpdate() {
  _nextUpdate = millis();
}

// Prints all forecasts to the serial monitor.
void ForecastObserver::weatherDataToLog() {
  for(int i = 0; i < NUM_OF_FORECASTS; i++) {
    if(_weatherData[i] != nullptr) {
      _weatherData[i]->printToLog();
    }
    else {
      Serial.print(F("! No weather data at index "));
      Serial.println(i);
    }
  }

  Serial.println(F("=========== Best Weather ==========="));
  getBestWeather()->printToLog();
  Serial.println(F("=========== Worst Weather ==========="));
  getWorstWeather()->printToLog();
}

// Returns weather info to be displayed as 16*2 characters.
void ForecastObserver::writeToLCD(rgb_lcd* lcd) {
  const int rowSize = 17;
  char topChars[rowSize];
  char bottomChars[rowSize];
  char* lcdMessage[] = { topChars, bottomChars };  // Should hopefully be char**

  int weatherChangeIndex = findWeatherChangeIndex();
  if(weatherChangeIndex != -1) {
    WeatherData* nextWeather = _weatherData[weatherChangeIndex];
    writeDynamicWeatherStatus(lcdMessage, rowSize, _weatherData[weatherChangeIndex]);
  }
  else {
    writeStaticWeatherStatus(lcdMessage, rowSize);
  } 

  // Write to 16x2
  lcd->setCursor(0, 1);
  for(int i = 0; i < 17; i++) {
    lcd->write(' ');
  }

  lcd->setCursor(0, 1);
  if(_rowToRead == 0) {
    lcd->write(topChars);
    _rowToRead = 1;
  }
  else {
    lcd->write(bottomChars);
    _rowToRead = 0;
  }
}

// From a given index, return the index at which the precipitation will change.
// If no precipitation is found, find the next hour of the day where cloudiness changes.
// Returns -1 if there's no weather change within the next NUM_OF_FORECASTS hours.
int ForecastObserver::findWeatherChangeIndex(int referenceIndex) {
  int referencePrecipitation = _weatherData[referenceIndex]->getCategoryValue();
  int referenceSeverity = _weatherData[referenceIndex]->getSeverityValue();
  int severityChangeIndex = -1;
  int precipitationChangeIndex = -1;
  int index = referenceIndex + 1;
  while(index < NUM_OF_FORECASTS) {
    int indexPrecipitation = _weatherData[index]->getCategoryValue();
    int indexSeverity = _weatherData[index]->getSeverityValue();
    if(indexPrecipitation != referencePrecipitation) {
      precipitationChangeIndex = indexPrecipitation;
      break;
    }

    // If no precipitation, check for nearest severity change (clear, cloudy, etc)
    if(severityChangeIndex == -1 && indexSeverity != referenceSeverity) {
      severityChangeIndex = index;
    }
    index++;
  }

  // If no precipitation, return change in cloud-conditions.
  return precipitationChangeIndex != -1 ? precipitationChangeIndex : severityChangeIndex;
}

// Fill buffer with weather information where weather conditions will change
void ForecastObserver::writeDynamicWeatherStatus(char** buffer, size_t size, WeatherData* nextForecast) {
  // Information of next forecast.
  // To be safe I store the strings locally, then call c_str().
  String forecastName = nextForecast->getWeatherName();
  const char* forecastNameCstr = forecastName.c_str();
  int forecastTime = nextForecast->getHour();
  
  // Information of current weather.
  String currentWeatherName = _weatherData[0]->getWeatherName();
  const char* currentWeatherNameCstr = currentWeatherName.c_str();
  float currentTemperature = _weatherData[0]->getTemperature();

  snprintf(buffer[0], size, "%s at %dH", forecastNameCstr, forecastTime);
  snprintf(buffer[1], size, "%s | %+.0fC", currentWeatherNameCstr, currentTemperature);
}

// Fill buffer with weather information where weather conditions will remain the same for all forecasts.
void ForecastObserver::writeStaticWeatherStatus(char** buffer, size_t size) {
  // Information of current weather.
  String currentWeatherName = _weatherData[0]->getWeatherName();
  const char* currentWeatherNameCstr = currentWeatherName.c_str();
  float currentTemperature = _weatherData[0]->getTemperature();

  snprintf(buffer[0], size, "%s all day", currentWeatherNameCstr);
  snprintf(buffer[1], size, "%s | %+.0fC", currentWeatherNameCstr, currentTemperature);
}

// Download forecast data from SMHI, deserialize and store information in WeatherData array attribute.
void ForecastObserver::updateData() {
  String longitudeStr = String(_longitude, 6);
  String latitudeStr = String(_latitude, 6);
  String dataUrl = String("http://opendata-download-metfcst.smhi.se/api/category/pmp3g/version/2/geotype/point/lon/" 
    + longitudeStr + "/lat/" 
    + latitudeStr + "/data.json");
  Serial.print(F("URL = "));
  Serial.println(dataUrl);

  WiFiClient wifiClient;
  HTTPClient httpClient;
  httpClient.useHTTP10(true);   // Might not be needed here.
  httpClient.begin(wifiClient, dataUrl);
  int httpCode = httpClient.GET();  // 200 = Amazing. 404 = Nothing here. Etc.
  Serial.println(httpCode);

  if(httpCode != HTTP_CODE_OK) {
    Serial.print(F("GET request failed: "));
    Serial.println(httpClient.errorToString(httpCode));
    return;
  }
  else {
    Serial.println(F("GET request succeeded!"));
  }
  
  WiFiClient* wifiStream = httpClient.getStreamPtr();
  
  // Filter that filters out irrelevant fields.
  JsonDocument filter;
  filter[0]["name"] = true;
  filter[0]["values"] = true;

  // Find referenceTime
  const int referenceStrLength = 20; // No terminator.
  char referenceTimeStr[referenceStrLength + 1];
  wifiStream->find("\"referenceTime\":\"");
  wifiStream->readBytesUntil('\"', referenceTimeStr, referenceStrLength);
  Serial.print(F("Reference Time: "));
  Serial.println(referenceTimeStr);

  // Convert referenceTime to hour, and offset from UTC to sweden's timezone
  int referenceHour = referenceTimeToHour(referenceTimeStr);
  referenceHour += _isSummertime ? SWEDEN_TIMEZONE_GMT + 1: SWEDEN_TIMEZONE_GMT;
  int forecastHour = referenceHour;
  Serial.print(F("Reference Hour: "));
  Serial.println(forecastHour);

  // Deserialize one "parameters" array at a time.
  int forecastIndex = 0;
  wifiStream->find("\"parameters\":");
  do {
    JsonDocument json;
    DeserializationError error = deserializeJson(json, *wifiStream, DeserializationOption::Filter(filter));
    if(error) {
      Serial.print(F("Deserialize error: "));
      Serial.println(error.c_str());

    }
    else if(json.isNull()) {
      Serial.println(F("Json is NULL"));
    }
    else {
      Serial.print(F("-- Creating forecast "));
      Serial.println(forecastIndex);
  
      // Soon-to-be WeatherData object attributes.
      forecastHour = forecastHour < 23 ? forecastHour + 1 : 0;  // Increment or wrap around to midnight.
      float temperature = 0.0f;
      int weatherCategory = 0;
  
      // Grab 't' and 'Wsymb2' from parameters and store them in a WeatherData object.
      int numOfParameters = 0;
      JsonArray parameters = json.as<JsonArray>();
      for(JsonObject parameter : parameters) {
        // Store values of relevant parameters.
        const char* paramName = parameter["name"];  // Max size 8 + 1
        if(strcmp(paramName, "t") == 0) {
          Serial.println(F("t found"));
          temperature = parameter["values"][0];
          numOfParameters++;
        }
        else if(strcmp(paramName, "Wsymb2") == 0) {
          Serial.println(F("Wsymb2 found"));
          weatherCategory = parameter["values"][0];
          numOfParameters++;
        }
  
        // Break when we have all the required parameters.
        if(numOfParameters == 2) {
          break;
        }
      }
  
      // Release memory of object that is being replaced. 
      // Should be done to prevent memory leaks.
      if(_weatherData[forecastIndex] != nullptr) {  // Need to assure WeatherData isn't being deleted elsewhere. Could lead to issues.
         delete _weatherData[forecastIndex];
      }
  
      Serial.print(forecastIndex + 1);
      Serial.print(F(" : Created for H"));
      Serial.println(forecastHour);
      
      _weatherData[forecastIndex] = new WeatherData(forecastHour, temperature, weatherCategory);
  
      // Only grab the required amount of forecasts.
      if(forecastIndex < NUM_OF_FORECASTS) {
        forecastIndex++;
      }
      else {
        break;
      }
    }
  } while(wifiStream->findUntil("\"parameters\":", "}]}]}"));   // Terminator is a longshot but the only shot afaik

  // Flush stream/wifiClient?
  wifiStream->flush();
  wifiClient.flush(); // Might be the exact same object.
  httpClient.end();
  wifiClient.stop();
  wifiStream->stop();

  weatherDataToLog();

  // Set time for next download.
  _nextUpdate = millis() + FORECAST_UPDATE_FREQUENCY_MS;
}

int ForecastObserver::referenceTimeToHour(const char* referenceTimeStr) {
  // referenceTime example: "2024-05-09T16:00:00Z"
  int i = 0;
  while(referenceTimeStr[i] != '\0') {
    if(referenceTimeStr[i] == 'T') {
      char hourChars[] = {
        referenceTimeStr[i + 1],
        referenceTimeStr[i + 2]
      };

      return atoi(hourChars); // Native function?
    }
    i++;
  }
}