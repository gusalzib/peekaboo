#ifndef ForecastObserver_h
#define ForecastObserver_h

#include "Arduino.h"
#include "WeatherData.h"
#include "ArduinoJson.h"
#include "rgb_lcd.h"

#define NUM_OF_FORECASTS 16
#define FORECAST_UPDATE_FREQUENCY_MS 1000 * 60 * 30  // 30 minutes
#define SWEDEN_TIMEZONE_GMT 1 

class ForecastObserver {
  private:
    // Had difficulties finding information of how to store a collection of object references as an attribute.
    // Asked BingAI/Copilot and it suggested the double pointer-type thing. It compiles, so I'm happy.
    // It did recommend using unique_ptrs, however. I chose to use this solution instead so that I could get more familiar with how pointers/references work in C/C++.
    // BingAI prompts: "In C++, how do I store an array as an object attribute?" and "I want the array to store references to MyClass objects"
    // Pointer/references discussion: https://stackoverflow.com/questions/2094666/pointers-in-c-when-to-use-the-ampersand-and-the-asterisk
    WeatherData** _weatherData;   // Pointer to a WeatherData pointer. WeatherData pointer == WeatherData array in this case. (Also, const * WeatherData* ?)
    float _longitude;
    float _latitude;
    unsigned long _nextUpdate;
    int _rowToRead;
    bool _isSummertime;
    void weatherDataToLog();
    WeatherData* getWorstWeather(int startIndex = 0);
    WeatherData* getBestWeather(int startIndex = 0);
    void writeDynamicWeatherStatus(char** buffer, size_t size, WeatherData* nextForecast);
    void writeStaticWeatherStatus(char** buffer, size_t size);
    bool getGeneralForecastStr(char buffer[]);
    int referenceTimeToHour(const char* referenceTimeStr);
    int getIncrementedHour(int hour);
    int findWeatherChangeIndex(int startIndex = 0);
  public:
    ForecastObserver(float longitude, float latitude);
    ~ForecastObserver();
    void setNewCoordinates(float longitude, float latitude);
    void updateData();
    bool readyToUpdate();
    void requestUpdate();
    float getCurrentTemperature();
    void writeToLCD(rgb_lcd* lcd);
    //String getAttireSuggestion();
};

#endif