#include "Arduino.h"
#include "WeatherData.h"
#include "WeatherCategory.h"

WeatherData::WeatherData(int hour, float temperature, int weatherCategory) {
  _hour = hour;
  _temperature = temperature;
  _weatherCategory = new WeatherCategory(weatherCategory);
}

WeatherData::~WeatherData() {
  delete _weatherCategory;
  // No manual deletion required since no attributes are made with 'new'?
  Serial.print(F("*!* WeatherData object destroyed *!* : "));
  Serial.println(_hour);
}

int WeatherData::getHour() {
  return _hour;
}

float WeatherData::getTemperature() {
  return _temperature;
}

String WeatherData::getWeatherName() {
  return _weatherCategory->getName();
}

int WeatherData::getCategoryValue() {
  return _weatherCategory->getCategoryValue();
}

int WeatherData::getSeverityValue() {
  return _weatherCategory->getSeverityValue();
}

bool WeatherData::hasPrecipitation() {
  return _weatherCategory->hasPrecipitation();
}

bool WeatherData::isConsideredBad() {
  return _weatherCategory->isConsideredBad();
}

// If THIS is "BETTER" return < 0, if "WORSE" return > 0, and 0 if equal.
int WeatherData::compare(WeatherData* otherWeather) {
  int thisCategory = _weatherCategory->getCategoryValue();
  int thisSeverity = _weatherCategory->getSeverityValue();
  int otherCategory = otherWeather->getCategoryValue();
  int otherSeverity = otherWeather->getSeverityValue();

  if(thisCategory == otherCategory) {
    if(thisSeverity == otherSeverity) {
      return 0;
    }
    else {
      return thisSeverity < otherSeverity ? -1 : 1;
    }
  }
  else {
    return thisCategory < otherCategory ? -1 : 1;
  }
}

void WeatherData::printToLog() {
  Serial.print(F("==="));
  Serial.print(F("Hour: "));
  Serial.println(_hour);
  Serial.print(F("\tTemperature: "));
  Serial.println(_temperature);

  // Hmm...
  Serial.print(F("\tWeather category: "));
  _weatherCategory->printToLog();  // This one might act up.
  Serial.println(F("---"));
}