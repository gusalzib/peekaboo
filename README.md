# PEƎKABOO
![Java](https://img.shields.io/badge/Java-ED8B00?style=for-the-badge&logo=java&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)

![Project Logo](app-code/peekApp/src/main/resources/image/logo.png)

## Introduction
PEƎKABOO is a product intended to aid the user in day-to-day life by gathering vital information to a place where we spend a lot of our time: the bathroom.
A lot of early mornings are spent in the bathrooms getting ready for the day, where we do our hair, shave, floss and brush our teeth. It's easy to lose track of time and fall behind on time in our daily routines. 

PEƎKABOO is a system with two components, namely a user application on a computer, and a "magic mirror" where time-sensitive information is displayed to the user. The mirror would display time, weather forecasts and to-do reminders. Information about bathroom temperature and humidity levels are also shown, and when these reach outside of desirable bounds the system can notify the user, for example so that build-up of mold can be prevented before it's too late. 

### Mirror Features
- Displaying temperature and humidity of the bathroom.
- Shows the current time on a built-in 16x2 LCD display.
- Timer for brushing teeth and washing hands (2-minute countdown timer).
- Clean and sleek mirror User Interface.
- Shows current weather and important forecast information.
- Shows user created reminders.
- The mirror turns on when users enter the bathroom, and turns off when they leave.

### Application Features
- Show the bathroom's current humidity levels and temperature in both fahrenheit and celsius.
- Clean and sleek application user interface.
- Show a notification and warn the user when the bathroom humidity is out of a specific range which could facilitate mold growth.
- Display the occupation status of the bathroom.
- Add or remove reminders that are shown on the mirror next time you enter the bathroom.
- Set the coordinates from the Settings screen so that the mirror could display relevant weather forecasts. 

## Technology Stack
- **Mirror Component (Wio Terminal):** C++ / Arduino. Uses MQTT for communication, and various sensors for environmental readings.
- **Application Component:** Java / JavaFX. Uses Maven for dependency management and MQTT to communicate with the mirror.

## Installation

### Hardware (Wio Terminal) Dependencies
In order to verify, compile and upload code to the Wio Terminal, you must ensure that you have the right dependencies. Using Arduino IDE, download the following libraries via the Library Manager:
- [rpcWiFi](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi)*
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [Seeed_Arduino_LCD](https://github.com/Seeed-Studio/Seeed_Arduino_LCD)
- [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)*
- [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Grove - LCD RGB Backlight](https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight)
- [Seeed Arduino RTC](https://github.com/Seeed-Studio/Seeed_Arduino_RTC)

_* has additional dependencies listed in the repository_

### User Application (Java) Dependencies
The Java application uses Maven for dependency management. To install and run the application:
1. Navigate to the `app-code/peekApp` directory.
2. Run `mvn clean install` to install dependencies (JavaFX, MQTT Client, Gson).
3. Run `mvn clean javafx:run` to launch the application.

