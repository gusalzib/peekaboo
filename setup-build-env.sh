# Credits to Pasha Klimenkov for writing esp32 CI guide: https://codeblog.dotsandbrackets.com/gitlab-ci-esp32-arduino/
# Credits as well to Nasit Vurgun for putting together and modifying this setup.
#!/bin/bash

apt-get update
cd ~

# Install arduino-cli
apt-get install curl -y
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
export PATH=$PATH:/root/bin
arduino-cli -version

# Installing Seeed Wio Terminal core libraries
printf "board_manager:\n  additional_urls:\n    - https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json\n" > .arduino-cli.yaml
arduino-cli core update-index --config-file .arduino-cli.yaml
arduino-cli core install Seeeduino:samd --config-file .arduino-cli.yaml

# Installing native libraries
arduino-cli lib install "Seeed Arduino rpcWiFi@1.0.6"
arduino-cli lib install "PubSubClient@2.8"
arduino-cli lib install "Seeed Arduino RTC@2.0.0"
arduino-cli lib install "DHT sensor library"
arduino-cli lib install "ArduinoJSON"
arduino-cli lib install "Adafruit NeoPixel"
arduino-cli lib install "Adafruit_NeoMatrix_ZeroDMA"
arduino-cli lib install "Adafruit ZeroDMA"
arduino-cli lib install "Adafruit DMA neopixel library"
arduino-cli lib install "Adafruit DotStarMatrix"
arduino-cli lib install "Seeed Arduino RTC"
arduino-cli lib install "Grove - LCD RGB Backlight"


cd -

# Installing non-native libraries
apt-get install git -y
cd `arduino-cli config dump | grep sketchbook | sed 's/.*\ //'`/libraries
git clone https://github.com/ThingPulse/esp8266-oled-ssd1306.git
git clone https://github.com/Seeed-Studio/Seeed_Arduino_RTC.git
git clone https://github.com/adafruit/Adafruit_Sensor.git
git clone https://github.com/adafruit/DHT-sensor-library # this library is not added properly. Both dht.h and dht.cpp
# files were included in the arduino source file.
#the following are dependency libraries for the motion sensor and rgb lights 
git clone https://github.com/adafruit/Adafruit_NeoPixel.git
git clone https://github.com/adafruit/Adafruit_NeoMatrix_ZeroDMA.git
git clone https://github.com/adafruit/Adafruit_ZeroDMA.git
git clone https://github.com/adafruit/Adafruit_NeoPixel_ZeroDMA.git
git clone https://github.com/adafruit/Adafruit-GFX-Library.git
git cone https://github.com/arduino-libraries/Arduino_JSON.git
git clone https://github.com/Seeed-Studio/Seeed_Arduino_RTC.git
git clone https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight.git

cd -