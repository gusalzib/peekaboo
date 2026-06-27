/*
  Built from the ground up.
  Four times as large.
  Sixteen times the detail.
  With help from a friendly reference: https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide/

    "What the gosh darn frick is this file?!"
  It's a "header" file, used to declare class methods and attributes. 
  No implementation is done here, as the
  actual implementation happens in WeatherData.cpp.
  All these files are merged together when compiled, .h files first, .cpp below,
  because C++ stuff is compiled from top to bottom.
  It just works (when it doesn't not work).
*/

#ifndef WeatherData_h
#define WeatherData_h

#include "Arduino.h"
#include "WeatherCategory.h"

/*
Don't go too specific on the clothing advice.
Separate the Temperature and Weather status.
Rain = Umbrella. Simple, clean, functional.
*/

enum class TemperatureComfort : int {
  FREEZING = 4,
  COLD = 12,
  TEMPERATURE = 18,
  WARM = 28
};

class WeatherData {
  private:
    // Wanted these as constants, but the compiler doesn't.
    int _hour;     // Format: "2024-05-01T08:00:00Z" Where T = idk, Z = ?, and timezone is UTC (GMT +0).
    float _temperature;
    WeatherCategory * _weatherCategory;
    //void convertTimeDate(String timedateStr);
  public:
    WeatherData(int hour, float temperature, int weatherCategory);
    ~WeatherData();
    int getHour();
    float getTemperature();
    int getCategoryValue();
    String getWeatherName(); 
    int getSeverityValue();
    bool hasPrecipitation();
    bool isConsideredBad();
    int compare(WeatherData* otherWeather);
    void printToLog();
};

#endif