// ---------------- Include other necessary files ----------------
// Include paired header file
#include "metro_tracker.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Arduino.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
//#include <FastLED.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "time.h"

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// Don't know what this does, but let's do it anyway
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif


// ---------------- Reference global variables declared elsewhere ----------------
// Matrix display variables
extern FastLED_NeoMatrix *matrix;
extern uint16_t colors[];
// WMATA server info
extern String wmataApiKey;
extern String StationCode;

// ---------------- Function ----------------
void metro_tracker() {
  // ---------------- SETUP ----------------

  // Manage display mode
  extern int display_mode;
  int og_display_mode = display_mode; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)
    
  // Configure display
  matrix->setBrightness(HIGH_BRIGHTNESS);
  matrix->setTextSize(1);
  int cursorx;
  
  // WMATA server info
  String ogStationCode = StationCode;
  // Variables to hold output data
  String jsonBuffer;
  int trainCount = 10;
  char westLines[trainCount][2];
  char westMinutes[trainCount][4];
  char eastLines[trainCount][2];
  char eastMinutes[trainCount][4];

  // Metro line colors
  String metroLines = "ZRBYOGS"; // Zero[null], Red, Blue, Yellow, Orange, Green, Silver. Cut down to 1 letter each in order to keep it as a 1d array
  uint16_t metroColors[7] = {matrix->Color(0,0,0),matrix->Color(255, 0, 0),matrix->Color(0, 0, 255),matrix->Color(255, 255, 0),matrix->Color(255, 165, 0),matrix->Color(0, 255, 0),matrix->Color(165, 165, 165)};


  // Set up web access variables
  WiFiClient client;
  HTTPClient http;


  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)
  while (1) {
    
    // Clear old data
    for(int i=0;i<trainCount;i++){
      for(int j=0;j<2;j++) westLines[i][j]=0;
      for(int j=0;j<4;j++) westMinutes[i][j]=0;
      for(int j=0;j<2;j++) eastLines[i][j]=0;
      for(int j=0;j<4;j++) eastMinutes[i][j]=0;
    }

    // Update path to WMATA API to get predictions for selected station code
    String metroServerPath = "http://api.wmata.com/StationPrediction.svc/json/GetPrediction/" + StationCode + "?api_key=" + wmataApiKey;

  // Receive updated metro data from weather server
    http.begin(client, metroServerPath.c_str());
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
    }
    else {
      
      // Iterate through all the incoming trains, assign them to Group 1 or 2 (East or West) and extract line/time data
      int iEast = 0;
      int iWest = 0;
      String group;
      for (int j=0;j<trainCount;j++){
        // Check for null entries in order to end the for loop
        if(JSON.stringify(myObject["Trains"][j]["Destination"]).equals("null")){
          break;
        }
        // Check for "No Passenger" trains and skip them
        if(JSON.stringify(myObject["Trains"][j]["Line"]).equals("\"No\"")){
          continue;
        }
        // Valid entries below
        // Split up trains by group
        group = JSON.stringify(myObject["Trains"][j]["Group"]).substring(1,2);
        // Group 1 = "West" (leftwards arrow)
        if (group.equals("1")){
          JSON.stringify(myObject["Trains"][j]["Line"]).substring(1,2).toCharArray(westLines[iWest],2);
          JSON.stringify(myObject["Trains"][j]["Min"]).substring(1,JSON.stringify(myObject["Trains"][j]["Min"]).length()-1).toCharArray(westMinutes[iWest],4);
          iWest++;
        }
        // Group 2 = "East" (rightwards arrow)
        else{
          JSON.stringify(myObject["Trains"][j]["Line"]).substring(1,2).toCharArray(eastLines[iEast],2);
          JSON.stringify(myObject["Trains"][j]["Min"]).substring(1,JSON.stringify(myObject["Trains"][j]["Min"]).length()-1).toCharArray(eastMinutes[iEast],4);
          iEast++;
        }
  
      }
    }

    // ---------------- DISPLAY ----------------
    
    matrix->fillScreen(LED_BLACK); // Erase previous content
    
    // Display all Westbound trains
    for (int iWest=0;iWest<2;iWest++){
      //Display colored arrow
      matrix->setCursor(0,iWest*8);
      matrix->setTextColor(metroColors[metroLines.indexOf(westLines[iWest])]);
      matrix->print("<-");
      //Display minutes to arrival
      if (atoi(westMinutes[iWest])<=0){
        cursorx = mw - 3*6;
      }
      else if (atoi(westMinutes[iWest])>9){
        cursorx = mw - 2*6;
      }
      else cursorx = mw - 6;
      matrix->setCursor(cursorx,iWest*8);
      matrix->setTextColor(colors[3]);
      matrix->print(westMinutes[iWest]);
    }
    
    // Display all Eastbound trains
    for (int iEast=0;iEast<2;iEast++){
      //Display colored arrow
      matrix->setCursor(0,iEast*8 + 2*8);
      matrix->setTextColor(metroColors[metroLines.indexOf(eastLines[iEast])]);
      matrix->print("->");
      //Display minutes to arrival
      if (atoi(eastMinutes[iEast])<=0){
        cursorx = mw - 3*6;
      }
      else if (atoi(eastMinutes[iEast])>10){
        cursorx = mw - 2*6;
      }
      else cursorx = mw - 6;
      matrix->setCursor(cursorx,iEast*8 + 2*8);
      matrix->setTextColor(colors[3]);
      matrix->print(eastMinutes[iEast]);
    }

    matrix->show();


    // ---------------- MODE MAINTENANCE/EVENT HANDLING ----------------
    // Remain in "button handling mode" for 20 seconds to avoid updating the metro info too often (the API limits number of calls per minute)
    for (int k=0;k<200;k++){
      // If the station code has changed, break the "button handling mode" pause
      if (ogStationCode != StationCode){
        ogStationCode = StationCode;
        break;
      }

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
      delay(100);
    }
    Serial.println("Refreshing Train info...");
  }

}
