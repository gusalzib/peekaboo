#ifndef WeatherCategory_h
#define WeatherCategory_h

#include "Arduino.h"

// Name and value matches that of Wsymb2 : https://opendata.smhi.se/apidocs/metfcst/parameters.html#parameter-wsymb
// Ordered in level of desirability. No Precipitation = :) , whereas Thunder = :(
// Simplify category to 5 levels (+ exception).
enum class GeneralCategory : int {
  UNKNOWN = -1,
  NO_PRECIPITATION = 0, // Wsymb2: 1, 2, 3, 4, 5, 6, 7
  SNOW,                 // Wsymb2: 15, 16, 17, 25, 26, 27
  SLEET,                // Wsymb2: 12, 13, 14, 22, 23, 24
  RAIN,                 // Wsymb2: 8, 9, 10, 18, 19, 20
  THUNDER               // Wsymb2: 11, 21
};

class WeatherCategory {
  private:
    int _severity;
    String _name;
    GeneralCategory _generalCategory;   // Rain + High Severity has higher weight than Sun when it comes to clothing.
    void setWeatherType(GeneralCategory category, int severity);
  public:
    WeatherCategory(int wsymb2Value);
    ~WeatherCategory();
    String getName();
    int getCategoryValue();
    int getSeverityValue();
    bool hasPrecipitation();
    bool isConsideredBad();
    void printToLog();
};

#endif
