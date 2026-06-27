#include "Arduino.h"
#include "WeatherCategory.h"

// Very important reference for the wsymb2Value to understand what's going on here: 
// https://opendata.smhi.se/apidocs/metfcst/parameters.html#parameter-wsymb
WeatherCategory::WeatherCategory(int wsymb2Value) {
  // Assign general precipitation category. 
  // Light rain showers and heavy rain is all rain in the end of the day.
  switch(wsymb2Value) {
    // --- No precipitation --- //
    case 1:
    case 2:
      _name = String(F("Clear"));
      setWeatherType(GeneralCategory::NO_PRECIPITATION, 0);
      break;
    case 3:
    case 4:
      _name = String(F("Cloudy"));
      setWeatherType(GeneralCategory::NO_PRECIPITATION, 1);
      break;
    case 5:
    case 6:
      _name = String(F("Overcast"));
      setWeatherType(GeneralCategory::NO_PRECIPITATION, 2);
      break;
    case 7:
      _name = String(F("Foggy"));
      setWeatherType(GeneralCategory::NO_PRECIPITATION, 3);
      break;
    // --- Rain --- //
    case 8:
    case 9:
    case 10:
    case 18:
    case 19:
    case 20:
      _name = String(F("Rain"));
      setWeatherType(GeneralCategory::RAIN, 0);
      break;
    // --- Sleet --- //
    case 12:
    case 13:
    case 14:
    case 22:
    case 23:
    case 24:
      _name = String(F("Sleet"));
      setWeatherType(GeneralCategory::SLEET, 0);
      break;
    // --- Thunder --- //
    case 11:
      _name = String(F("Storm"));
      setWeatherType(GeneralCategory::THUNDER, 0);
      break;
    case 21:
      _name = String(F("Thunder"));
      setWeatherType(GeneralCategory::THUNDER, 0);
      break;
    // --- Snow --- //
    case 15:
    case 16:
    case 17:
    case 25:
    case 26:
    case 27:
      _name = String(F("Snow"));
      setWeatherType(GeneralCategory::SNOW, 0);
      break;
    default:
      Serial.print(F("! Wsym2 integer not valid: "));
      Serial.println(wsymb2Value);
      _name = String(F("Unknown"));
      setWeatherType(GeneralCategory::UNKNOWN, -1);
      break;
  }
}

WeatherCategory::~WeatherCategory() {
  Serial.print(F("Deleting "));
  Serial.println(_name);
}

void WeatherCategory::setWeatherType(GeneralCategory category, int severity) {
  _generalCategory = category;
  _severity = severity;
}

String WeatherCategory::getName() {
  return _name;
}

int WeatherCategory::getCategoryValue() {
  return static_cast<int>(_generalCategory);
}

int WeatherCategory::getSeverityValue() {
  return _severity;
}

bool WeatherCategory::hasPrecipitation() {
  return static_cast<int>(_generalCategory) > static_cast<int>(GeneralCategory::NO_PRECIPITATION);
}

bool WeatherCategory::isConsideredBad() {
  // At the time of implementation, the two constants below refer to "moderate snow showers".
  // Anything below the int value of badCategoryLowerBound is considered as passable weather.
  // Anything less than "severityLowerBound" is not considered bad weather, 
  // IF AND ONLY IF GeneralCategory == badCategoryLowerBound.
  const int badCategoryLowerBound = static_cast<int>(GeneralCategory::SNOW);
  const int severityLowerBound = 2; // Severity is for now only applicable to GeneralCategory::NO_PRECIPITATION

  if(getCategoryValue() >= badCategoryLowerBound) {
    return true;
  }
  else {
    return _severity >= 2;
  }
}

void WeatherCategory::printToLog() {
  Serial.print(_name);
  Serial.print(F(" | "));
  Serial.print(F("Category value: "));
  Serial.print(getCategoryValue());
  Serial.print(F(" | Severity: "));
  Serial.println(_severity);
}
