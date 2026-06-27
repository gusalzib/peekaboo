//this code was made possible by following the tutorial on
// https://wiki.seeedstudio.com/Wio-Terminal/

//this code was done with help from the guide on the link below:
//https://arduinogetstarted.com/tutorials/arduino-temperature-humidity-sensor
#include <Arduino.h>
#include "DHT.h"
#include "TFT_eSPI.h"
#include "rpcWiFi.h"          // Docs kinda: https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi/tree/master/src
#include <WiFiClientSecure.h>
#include <PubSubClient.h>     // Docs: https://pubsubclient.knolleary.net/api
#include <HTTPClient.h>       // Used to make HTTPS requests. Repo .h file https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi/blob/master/src/HTTPClient.h
#include <ArduinoJson.h>
#include <rgb_lcd.h>

#include "WeatherData.h"
#include "ForecastObserver.h"
#include "Adafruit_NeoPixel.h"
#include "ArduinoJson.h"
#include <Wire.h>
#include "RTC_SAMD51.h"
#include "DateTime.h"
#include <rgb_lcd.h>

#define PIN 6 // RGB port
#define NUMPIXELS 10 // RGB related
#define PIR_PinNum 2 // for motion sensor port
#define PinNum 0 // specifying which pin
#define SensorType DHT11 // specifying type of DHT as our sensor is DHT11 
#define LCD_BACKLIGHT (72Ul) //Control pin of LCD
#define SYSTEM_SLEEP_INTERVAL_MS 3000
#define TEXT_ROTATION_INTERVAL_MS 5000
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS,PIN, NEO_RGB + NEO_KHZ800);
int delayval = 100;

const char* WiFiName = "Jey";
const char* password = "jazzy blues!";
const char* mqttBroker = "0ddbb8a8e3914d0481803c9e3aa48969.s1.eu.hivemq.cloud";
const char* brokerUsername = "peekapp";
const char* brokerPassword = "Peekapp@1";
const char* clientID = "wio-test1";
const int brokerPort = 8883;

//MQTT Broker Topics
const char* GENERAL_TOPIC = "bathroom/general";  
const char* CELSIUS_TOPIC = "bathroom/celsius";
const char* FAHRENHEIT_TOPIC = "bathroom/fahrenheit";
const char* HUMIDITY_TOPIC = "bathroom/humi";
const char* LOCATION_TOPIC = "bathroom/location";
const char* REMINDER_TOPIC = "reminder";
const char* MOTION_TOPIC = "bathroom/motion";

float userLongitude = 11.970401;  // Default values
float userLatitude = 57.708424;   // Default values

int humidity = 0;
int temperatureC = 0;
int temperatureF = 0;
const int maxReminders = 3;
int reminderNoteDelay = 10000;
int previousReminderMillis = 0;
int reminderIndex = 0;

char* bathroomAvailability;

// Millis timestamps
long sleepThresholdMillis = millis() + SYSTEM_SLEEP_INTERVAL_MS;
long weatherTextRotationMillis = millis() + TEXT_ROTATION_INTERVAL_MS;

RTC_SAMD51 rtc;
rgb_lcd lcd;
TFT_eSPI tft;
WiFiClientSecure hiveMqWifiClient;
PubSubClient mqttClient(hiveMqWifiClient);
DHT tempAndHumSensor(PinNum, SensorType);
String remindersArray[maxReminders + 1] = {};  // Be wary of Strings
ForecastObserver* forecastObserver;

// Downloaded .pem file from HiveMQ and copy-pasted contents. ~2kb's worth of text.
const char* hiveMqCAcert = 
  "-----BEGIN CERTIFICATE-----\n"
  "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\n"
  "-----END CERTIFICATE-----";

void setup() {
  pinMode(WIO_BUZZER, OUTPUT);
  Serial.begin(9600);
  while(!Serial) //wait for serial connection to be ready
  pixels.begin();
  tempAndHumSensor.begin();
  setupWiFi();
  setupLCD();
  setupTFT();
  
  //Setup server and connects to the broker
  setupMQTTConnection();
  subscribe(GENERAL_TOPIC);
  subscribe(REMINDER_TOPIC);
  mqttClient.setCallback(callback);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(PIR_PinNum, INPUT);

  // Save user location to flash, and default to something or skip?
  forecastObserver = new ForecastObserver(userLongitude, userLatitude);
  subscribe(LOCATION_TOPIC);   // Subscribe after initialization, else the callback might make calls to non-existing object.

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to wifi. Reconnecting...");
    setupWiFi();
  }
  if(!mqttClient.connected()) {
    Serial.println("Not connected to broker. Reconnecting...");
    connectToBroker();
  }
}

// May need to check MQTT connection and reconnect if connection is broken?
void loop() {
  // If motion is detected, set the next time that the system will sleep (now + constant)
  if(motionDetected()) {
    sleepThresholdMillis = millis() + SYSTEM_SLEEP_INTERVAL_MS;
  }

  if(sleepThresholdMillis > millis()) {
    // All actions that are to take place exclusively when system is "active"
    bathroomAvailability = "occupied";
    publish(MOTION_TOPIC, bathroomAvailability);

    RGBLed(150,0,0);
    digitalWrite(LCD_BACKLIGHT, HIGH);
    showTimeLoop();
    keypadLoop();
    if(weatherTextRotationMillis < millis()) {
      forecastObserver->writeToLCD(&lcd);
      weatherTextRotationMillis = millis() + TEXT_ROTATION_INTERVAL_MS;
    }
  } else {
    // All actions that are to take place exclusively when system is "inactive"
    bathroomAvailability = "available"; 
    publish(MOTION_TOPIC, bathroomAvailability);    
    RGBLed(0,0,0);
    digitalWrite(LCD_BACKLIGHT, LOW);

    // Update forecasts after ForecastObserver::UPDATE_FREQUENCY_MS
    // Done when system is inactive to avoid unresponsiveness (TBD)
    if(forecastObserver->readyToUpdate()) {
      forecastObserver->updateData();
    }
  }

  // All actions that take place regardless of system inactivity below.
  updateBathroomMeasurements();
  mqttClient.loop();
  displayReminders();
}

void setupLCD() {
  lcd.begin(16, 2);
}

bool motionDetected() {
  if(digitalRead(PIR_PinNum)) {
    return true;
  }
  else {
    return false;
  }
}

void updateBathroomMeasurements() {
  //read humidity
  humidity = tempAndHumSensor.readHumidity();
  //read temperature in Celsius
  temperatureC = tempAndHumSensor.readTemperature();
  // read temperature in Fahernheit
  temperatureF = tempAndHumSensor.readTemperature(true);
  
  //nan stands for "not a number", so, isnan represents an 
  //undefined or unrepresentable value in floating point arithmetic
  if(isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println("failed to read from the sensor...");
  } else {
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Temperature", 25, 55);
    tft.drawString("Humidity", 200, 55);

    tft.setTextSize(2);
    tft.drawString("C", 55, 105);
    tft.drawString("F", 140, 105);
    tft.drawString("%", 255, 105);
    tft.fillRect(25, 100, 28, 25, TFT_LIGHTGREY);
    tft.drawNumber(temperatureC,30,105); //Display temperature values
    tft.fillRect(110, 100, 28, 25, TFT_LIGHTGREY);
    tft.drawNumber(temperatureF,115,105); //Display temperature values
    tft.fillRect(225, 100, 28, 25, TFT_LIGHTGREY);
    tft.drawNumber(humidity,230,105); //Display sensor values as percentage
    delay(500);
    tft.drawString("C", 55, 105);
    tft.drawString("F", 140, 105);
    tft.drawString("%", 255, 105);

    // Turn float values into c strings and publish
    char tempCStr[4]; 
    char tempFStr[4];
    char humidityStr[4];
    dtostrf(temperatureC, 3, 1, tempCStr);
    dtostrf(temperatureF, 3, 1, tempFStr);
    dtostrf(humidity, 3, 1, humidityStr);
    publish(CELSIUS_TOPIC, tempCStr);
    publish(FAHRENHEIT_TOPIC, tempFStr);
    publish(HUMIDITY_TOPIC, humidityStr);
  }
}

void setupTFT() { 
  tft.begin(); //Start TFT LCD
  tft.setRotation(3); //Set TFT LCD rotation
  tft.fillRect(0, 0, 320, 240, TFT_MAROON);
  printStartingLogo("PE3KABOO");
  printStartingLogo("PE3KABOO.");
  printStartingLogo("PE3KABOO..");
  printStartingLogo("PE3KABOO...");
  tft.fillRect(220, 120, 60, 25, TFT_MAROON);
  printStartingLogo("PE3KABOO.");
  printStartingLogo("PE3KABOO..");
  printStartingLogo("PE3KABOO...");
  setupWiFi();
  delay(2000);
  tft.fillRect(0, 0, 320, 240, TFT_LIGHTGREY);
  tft.fillRect(0, 0, 320, 50, TFT_BLACK);
  tft.setTextColor(TFT_BLUE); //Setting text color
  tft.setTextSize(3); //Setting text size
  tft.drawString("PE3KABOO", 90, 15); //Drawing a text string

  tft.drawFastVLine(90,85,55,TFT_BLACK); //Drawing verticle line
  tft.drawFastVLine(180,85,55,TFT_BLACK); //Drawing verticle line
  tft.drawFastHLine(0,140,320,TFT_BLACK); //Drawing horizontal line
  tft.drawFastHLine(0,85,320,TFT_BLACK); //Drawing horizontal line

}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("connecting to wifi ");
  WiFi.begin(WiFiName, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    WiFi.begin(WiFiName, password);
  }
  Serial.println("Connected to the WiFi network!");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}


void setupMQTTConnection(){
  // Setup server and connects to the broker
  mqttClient.setServer(mqttBroker, brokerPort);
  connectToBroker();
}

void connectToBroker() {
  // set certificate of Certificate Authority. (Wants "rootCA").
  hiveMqWifiClient.setCACert(hiveMqCAcert);

  while(!mqttClient.connected()){
    mqttClient.connect(clientID, brokerUsername, brokerPassword);

    if(mqttClient.connected()){
      Serial.println("Connected to broker!"); //exits loop

      // Publish confirmation message that the client has connected.
      // TODO: Include clientID in payload below, without starting a massive fire
      const char* payload = "Broker has been connected to by ";
      publish(GENERAL_TOPIC, payload);
    } else {
      //shows the error code and tries to reconnect
      Serial.printf("The connection failed. The error code is: %d Refer to the API documentation. Trying to reconnect...%n" , mqttClient.state());
    }
  }
}

// Internal loop if publish QoS < 1?
bool publish(const char* topic, const char* payload) {
  // Attempt publish and store result.
  bool publishSuccess = mqttClient.publish(topic, payload);

  // Print result and MQTT connection state if publish failed.
  if(!publishSuccess) {
    const char* publishStatus = publishSuccess ? "Publish succeeded." : "Publish failed. MQTT State = ";
    Serial.print("Publish failed. MQTT State = ");
    Serial.println(mqttClient.state());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  for(int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // 
  if(strcmp(topic, LOCATION_TOPIC) == 0) {
    // Payload format strictness is high with this one.
    const int coordinateSize = 9; // 2 integers followed by 1 dot and 6 decimals (xx.xxxxxx)
    char latitude[coordinateSize + 1];  // +1 for terminating character.
    char longitude[coordinateSize + 1];
    for(int i = 0; i < coordinateSize; i++) {
      // Parse concurrently
      latitude[i] = (char)payload[i];
      longitude[i] = (char)payload[coordinateSize + i];
    }

    userLatitude = (float)atof(latitude);
    userLongitude = (float)atof(longitude);
    Serial.print("New latitude = ");
    Serial.print(userLatitude);
    Serial.print("New longitude = ");
    Serial.print(userLongitude);
    
    forecastObserver->setNewCoordinates(userLongitude, userLatitude);
    forecastObserver->requestUpdate();
  }
  else if(strcmp(REMINDER_TOPIC, topic) == 0) {
    // Write bytes to array as chars
    char remindersJson[length + 1] = {}; // +1 for terminating character. HOPEFULLY zero-initializes.
    for(int i = 0; i < length; i++) {
      remindersJson[i] = (char)payload[i];
    }

    // Deserialize resulting Json and assign to global remindersArray.
    deserializeReminders(remindersJson);
  }
}

bool subscribe(const char* topic) {
  bool subscribeSuccess = mqttClient.subscribe(topic);
  const char* subscribeStatus = subscribeSuccess ? "Subscription succeeded" : "Subscription failed";
  Serial.println(subscribeStatus);
  return subscribeSuccess;
}

void RGBLed (int green, int red, int blue){
  for (int i = 0; i < NUMPIXELS; i++){
    pixels.setPixelColor(i, pixels.Color(green, red, blue));
    pixels.show();
    delay(delayval);
  }
}

void printStartingLogo(String message) {
    tft.setTextColor(TFT_BLUE); //Setting text color
    tft.setTextSize(3.4); //Setting text size
    tft.drawString(message, 75, 120); //Drawing a text string
    delay(1000);
}

bool deserializeReminders(const char* data) {
  JsonDocument jsonDoc;

  // Attempt deserialization, log and abort if error occurs.
  DeserializationError error = deserializeJson(jsonDoc, data);
  if(error) {
    Serial.print(F("Error deserializing json: "));
    Serial.println(error.c_str());
    return false;
  }

  // Extract values from each key-value pair and assign to remindersArray.
  int reminderIndex = 0;
  for(JsonVariant keyValue : jsonDoc.as<JsonArray>()) {
    if(reminderIndex == maxReminders) {
      Serial.println(F("Exceeding max reminders. Aborting."));
      break;
    }
    // Strings could end up being difficult to work with, but const char* tricky to assign.
    remindersArray[reminderIndex] = keyValue["text"].as<String>();
    reminderIndex++;
  }

  // Clear rest of the reminders if one was removed
  clearRemindersFrom(reminderIndex);
  return true;
}

void clearRemindersFrom(int startIndex) {
  for(int i = startIndex; i < maxReminders; i++) {
    remindersArray[i] = ""; // A guess.
  }
}

void displayReminders() {
  String reminderMessage;
  int currentReminderMillis = millis();
    if (currentReminderMillis - previousReminderMillis > reminderNoteDelay
    && remindersArray[reminderIndex].compareTo("") != 0) {
        tft.setTextSize(2);
        tft.fillRect(15, 170, 325, 25, TFT_LIGHTGREY);
        tft.drawString(remindersArray[reminderIndex], 20, 175);

        Serial.println(remindersArray[reminderIndex]);
        reminderMessage = remindersArray[reminderIndex];
        previousReminderMillis = currentReminderMillis;
        reminderIndex++;
    } else if (remindersArray[reminderIndex].compareTo("") == 0
      || reminderIndex == maxReminders) {
      reminderIndex = 0;
    }
}

void showTimeSetup(){
  rtc.begin();
  //Get date and time from compilation time
  DateTime now = DateTime(F(__DATE__), F(__TIME__));
  rtc.adjust(now);
}

void showTimeLoop(){
  //set where the time is displayed on 16x2 display
  lcd.setCursor(0,0);
  //get the current time
  DateTime now = rtc.now();
  //print the time
  lcd.print("TIME:");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
}

void keypadLoop(){
  bool loop = false;
  int countdown = 120;
  //if key A is pressed the timer starts
  if(digitalRead(WIO_KEY_A) == LOW){
    loop = true;
    while(loop){
      //if keys B or C are pressed, it exits the loop
      if((digitalRead(WIO_KEY_B) == LOW) || (digitalRead(WIO_KEY_C) == LOW)){
        clearDisplay();
        loop = false;
      }
      //if countdown !0, it continues to decrement the countdown. Otherwise, it exits the loop
      if (countdown == 0){
        clearDisplay();
        buzz();
        lcd.print("Time's up");
        delay(2000);
        clearDisplay();
        loop = false;
      }
      else {
        clearDisplay();
        printCountdown(countdown);
        //one second delay for decrementing the countdown
        delay(1000);
        countdown--;
      }
    }
  }
  //if Key B is pressed
  if (digitalRead(WIO_KEY_B) == LOW){
    clearDisplay();
    lcd.print("Key B pressed!");
  }
  //if Key C is pressed
  if (digitalRead(WIO_KEY_C) == LOW){
    clearDisplay();
    lcd.print("Key C pressed!");
  }
}

void printCountdown(int timer){
  if (timer>=100){
    clearDisplay();
    lcd.print(timer);
  }
  //Clear LCD when the count goes from 3 digits to 2 digits
  if(timer > 1 && timer < 100){
      clearDisplay();
      lcd.print(timer);
  }
}

//Clean the lcd and show the time constantly on cursor (0,0)
void clearDisplay(){
  lcd.clear();
  showTimeLoop();
  lcd.setCursor(0,1);
}

//Set buzzer for one second when timer reaches 0
void buzz(){
  analogWrite(WIO_BUZZER, 128);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);
  delay(1000);
}
