// ---------------- Include other necessary files ----------------
// Include paired header file
#include "clock_display.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Arduino_JSON.h>
#include <FastLED_NeoMatrix.h>
#include <HTTPClient.h>
#include "time.h"

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// Don't know what this does, but let's do it anyway
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// ---------------- Function ----------------
void clock_display() {

  // ---------------- SETUP ----------------
  
  // Manage display mode
  extern int display_mode;
  int og_display_mode = display_mode; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Configure display
  extern FastLED_NeoMatrix *matrix;
  extern uint16_t colors[];
  int x    = 0;
  matrix->setBrightness(HIGH_BRIGHTNESS);
  matrix->setTextSize(1);
  int dateTextLength = 12; // Only scrolling text is the date
  int pixel_width = dateTextLength * 6 * 1;

  // Time server info
  const char* ntpServer = "pool.ntp.org";
  extern int gmtOffset_hours;
  const int  gmtOffset_sec = gmtOffset_hours * 3600;
  const int   daylightOffset_sec = 3600;

  // Weather server info
  extern String openWeatherMapApiKey;
  extern String city;
  extern String countryCode;
  String weatherServerPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
  String jsonBuffer;
  char tempchar [6] = "";
  int tempFahrenheit = 0;
  // Weather update info
  int display_refresh_delay = 300; // milliseconds
  int weather_update_time = 60; // seconds
  int weather_counter = 0;
  int weather_updateCount = floor(weather_update_time*1000/display_refresh_delay);


  // ---------------- COLLECT/PROCESS INITIAL DATA ----------------

  // Receive initial time from time server
  int ServerRequests = 0;
  bool ValidTime = true;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    ServerRequests++;
    Serial.println("Failed to obtain time");
    matrix->drawPixel(ServerRequests, 0, matrix->Color(255, 255, 255));
    delay(500);
    if (ServerRequests > 10) {
      ValidTime = false;
      break;
    }
  }

  // Receive initial weather data from weather server
  WiFiClient client;
  HTTPClient http;
  http.begin(client, weatherServerPath.c_str());
  int httpResponseCode = http.GET();

  jsonBuffer = "{}";
  if (httpResponseCode > 0) {
    jsonBuffer = http.getString();
  }
  // Free resources
  http.end();
  // Parse data
  JSONVar myObject = JSON.parse(jsonBuffer);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    strcat(tempchar, "xxxx");
  }
  else {
    // Define tempchar
    double tempKelvin = myObject["main"]["temp"];
    tempFahrenheit = round((tempKelvin-273.15)*9/5 +32);
    sprintf(tempchar,"%d F",tempFahrenheit);
  }
  
  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)
  while (1) {
    // ---------------- COLLECT/PROCESS REPEATING DATA ----------------
    // Process Time Data
    // Determine the time string
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeHour[3];
    strftime(timeHour, 3, "%I", &timeinfo); // Hours (12-hour format)
    char timeMinute[3];
    strftime(timeMinute, 3, "%M", &timeinfo);
    char timeSecond[3];
    strftime(timeSecond, 3, "%S", &timeinfo);
    char timeAMPM[3];
    strftime(timeAMPM, 3, "%p", &timeinfo);
    char timechar[14];
    strcpy(timechar, timeHour);
    strcat(timechar, ":");
    strcat(timechar, timeMinute);
    strcat(timechar, ":");
    strcat(timechar, timeSecond);
    strcat(timechar, timeAMPM);

    // Display time
    matrix->fillScreen(0);
    matrix->setTextColor(colors[3]);
    matrix->setTextWrap(true);
    matrix->setCursor(1, 0);
    matrix->print(timechar);

    // Determine the date string
    char dateWeekday[4];
    strftime(dateWeekday, 4, "%a", &timeinfo);
    char dateDate[8];
    strftime(dateDate, 9, "%D", &timeinfo);
    char datechar[14];
    strcpy(datechar, dateWeekday);
    strcat(datechar, " ");
    strcat(datechar, dateDate);

    // UPDATE WEATHER DATA - periodically
    // Use counter to determine when it is time to update weather
    if(weather_counter>weather_updateCount){
      weather_counter = 0;
      Serial.println("Updating weather info");
      http.begin(client, weatherServerPath.c_str());
        int httpResponseCode = http.GET();
      
        jsonBuffer = "{}";
        if (httpResponseCode > 0) {
          jsonBuffer = http.getString();
        }
        // Free resources
        http.end();
        // Parse data
        JSONVar myObject = JSON.parse(jsonBuffer);
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
          strcat(tempchar, "xxxx");
        }
        else {
          double tempKelvin = myObject["main"]["temp"];
          tempFahrenheit = round((tempKelvin-273.15)*9/5 +32);
          sprintf(tempchar,"%d F",tempFahrenheit);
        }
    }

    // ---------------- DISPLAY ----------------
    
    // Display time (text wrapped)
    // Note: this is done a few dozen lines up, and it breaks if you move the display code down here for some reason
    
    // Display date (scrolling)
    matrix->setTextColor(colors[2]);
    matrix->setTextWrap(false);
    matrix->setCursor(x, 16);
    matrix->print(datechar);
    x--;
    if (x < -pixel_width) x = mw;

    // Display temperature (static)
    matrix->setTextColor(colors[1]);
    // right-align the text based on # of digits in the temp.
    int tempx = mw - (4*6);
    if (tempFahrenheit > 99){
      tempx = mw - (5*6);
    }
    matrix->setCursor(tempx, 24);
    matrix->print(tempchar);
    // Manually print a degree sign (2x2 square) - easier than implementing a custom font :)
    matrix->drawRect(mw-6-4,24,2,2,colors[1]);

    matrix->show();

    // ---------------- MODE MAINTENANCE/EVENT HANDLING ----------------
    // If display mode is different from the og value or mode button is pressed, end the loop
    if (display_mode != og_display_mode) {
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;
    }
    else if (digitalRead(BUTTONPIN)) {
      display_mode++;
      while (digitalRead(BUTTONPIN)) {}
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;
    }
    // Prepare for next iteration of loop
    delay(display_refresh_delay);
    weather_counter++;
  }

}
