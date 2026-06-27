# PEƎKABOO
![Project Logo](app-code/peekApp/src/main/resources/image/logo.png)
<!-- Readme guide: https://www.makeareadme.com/ -->
## Introduction
PEƎKABOO is a product intended to aid the user in day-to-day life by gathering vital information to a place where we spend a lot of our time; The bathroom.
A lot of early mornings are spent in the bathrooms getting ready for the day, where we do our hair, shave, floss and brush our teeth. It's easy to lose track of time and fall behind on time in our daily routines. 

PEƎKABOO is a system with two components, namely a user application on a computer, and a "magic mirror" where time-sensitive information is displayed to the user. The mirror would display time, weather forecasts and to-do reminders. Information about bathroom temperature and humidity levels are also shown, and when these reach outside of desirable bounds the system can notify the user, for example so that build-up of mold can be prevented before it's too late. 

### Mirror Features <!-- (HTML comment) Double check that all features are here. -->
- Displaying temperature and humidity of the bathroom.
- Shows the current time on a built-in 16x2 LCD display.
- Timer for brushing teeth and washing hands (2-minute countdown timer).
- Clean and sleet mirror User Interface.
- Shows current weather and important forecast information.
- Shows user created reminders.
- The mirror turns on when users enter the bathroom, and turns off when they leave.

### Application Features
- Show the bathroom's current humidity levels and temperature in both fahrenheit and celsius.
- Clean and sleet application user interface.
- Show a notification and warn the user when the bathroom humidity is out of a specific range which could facilitate mold growth.
- Display the occupation status of the bathroom.
- Add or remove reminders that are shown on the mirror next time you enter the bathroom.
- Set the coordinates from the Settings screen so that the mirror could display relevant weather forecasts. 

## Installation
### Library dependencies'
In order to verify, compile and upload code to the Wio Terminal, you must ensure that you have the right dependencies. Using Arduino IDE, download the following libraries via the Library Manager:
- [rpcWiFi](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi)*
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [Seeed_Arduino_LCD](https://github.com/Seeed-Studio/Seeed_Arduino_LCD)
- [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)*
- [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Grove - LCD RGB Backlight](https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight)
- [DHT Sensor library](https://github.com/adafruit/DHT-sensor-library)*
- [Seeed Arduino RTC](https://github.com/Seeed-Studio/Seeed_Arduino_RTC)<!--Double check this is the correct one-->

For the user application side of the system, using IntelliJ:
- [Install guide](https://www.youtube.com/watch?v=fC7oUOUEEi4&pp=ygULc3RpY2tidWdnZWQ%3D)


_* has additional dependencies listed in the repository_
<!--
Don't know about Java-side dependencies, nor if there are any more dependencies for the Wio. Will have to come back to this periodically
-->

## How does it work?


## Usage: 

how to interact with the device?

## Future Plans for PEƎKABOO:
Any features planned for the future? Possible developments for existing product?

## Limitations: 

