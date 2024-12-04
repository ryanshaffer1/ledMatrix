// ---------------- Include other necessary files ----------------
// Include paired header file
#include "handle_wifi.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include <FastLED_NeoMatrix.h>

// Don't know what this does, but let's do it anyway
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

// ---------------- Reference global variables declared elsewhere ----------------
extern const char* ssid;
extern const char* password;
extern const char* domain_name;
extern String StationCode;
extern String input_text;
extern uint16_t textcolor;
extern uint8_t text_size;
extern int text_scroll;
extern bool game_mode;
extern bool fire_stoked;
extern String image_filename;
extern bool updateImage;
extern int display_mode_int;
extern char display_modes[][6];
extern int num_display_modes;

// ---------------- Declare variables for functions below ----------------
AsyncWebServer server(80); //Set web server port number to 80
String string_game_mode;
char char_game_mode[2];
String mode_title;
String getContentType(String filename);
bool handleFileRead(String path);

// ---------------- Helper Function initWiFi ----------------
// Initializes the ESP32's WiFi connection as a station on the local network. Sets up MDNS to allow for easier pings/connections across the network
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED){
      Serial.print('.');
      delay(1000);
    }
    Serial.println(WiFi.localIP());
    Serial.println("RRSI: ");
    Serial.println(WiFi.RSSI());

    // MDNS setup
    if (!MDNS.begin(domain_name)) {
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
  
  }

// ---------------- Helper Function handleNotFound----------------
// Throws 404 error for any requests for pages that cannot be found
void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  }

// ---------------- Function ----------------
void handle_wifi() {
  // ---------------- Setup ----------------

  // Initialize WiFi
  initWiFi();

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  // ---------------- Serve Web Pages & Files upon Request ----------------

  // HTML FILES
  // Send index.html (main web page) to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/index.html");
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/index.html");
  });

  // Send clock.html to client
  server.on("/clock.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"clock")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/clock.html");
  });
  
  // Send metrotracker.html to client
  server.on("/metrotracker.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"metro")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/metrotracker.html");
  });

  // Send textdisplay.html to client
  server.on("/textdisplay.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"text_")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/textdisplay.html");
  });

  // Send fireplace.html to client
  server.on("/fireplace.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"firep")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/fireplace.html");
  });

  // Send fireworks.html to client
  server.on("/fireworks.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"firew")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/fireworks.html");
  });
  
  // Send musicvis.html to client
  server.on("/musicvis.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"music")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/musicvis.html");
  });
  
  // Send imagedisplay.html to client
  server.on("/imagedisplay.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"image")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/imagedisplay.html");
  });
  
  // Send groovy.html to client
  server.on("/groovy.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"groov")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/groovy.html");
  });

  // Send dvdbounce.html to client
  server.on("/dvdbounce.html", HTTP_GET, [](AsyncWebServerRequest *request){
    for(int i = 0; i<num_display_modes; i++){
      if(!strcmp(display_modes[i],"bounc")){
        display_mode_int = i;
      }
    }
    request->send(SPIFFS,"/dvdbounce.html");
  });  
  
  // HELPER FILES
  // Send logo.png (site logo) to client
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Logo.PNG", "image/png");
  });
  
  // userConfig.txt (user configuration/settings) to client
  server.on("/userConfig.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/userConfig.txt", "text/plain");
  });

  // all_display_modes.txt (list of valid display modes) to client
  server.on("/all_display_modes.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/all_display_modes.txt", "text/plain");
  });

  // CSS
  // Send styles.css (formatting file) to client
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  // JAVASCRIPT
  // Send main.js (scripting file) to client
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.js", "text/javascript");
  });
  // Send draw_image.js (drawing image processing file) to client
  server.on("/draw_image.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/draw_image.js", "text/javascript");
  });
  // Send upload_image.js (image upload/processing file) to client
  server.on("/upload_image.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/upload_image.js", "text/javascript");
  });

  // Send utility.js (helper functions) to client
  server.on("/utility.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/utility.js", "text/javascript");
  });



  // ---------------- Input Handling ----------------
  
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {

  // Make updates to internal variables when new Metro Station is selected
  if (request->hasParam("MetroSelector")){
    StationCode = request->getParam("MetroSelector")->value();
    Serial.println(StationCode);
    request->send(SPIFFS,"/metrotracker.html");
  }
  
  // Make updates to internal variables when new Text is entered
  if (request->hasParam("FreeText")) {
    input_text = request->getParam("FreeText")->value();
    text_size = (request->getParam("fontsize")->value()).toInt();
    text_scroll = (request->getParam("wrapBool")->value()).toInt();
    textcolor = (request->getParam("textcolor")->value()).toInt();
    Serial.println(textcolor);
    request->send(SPIFFS,"/textdisplay.html");
  }
  
  // Make updates to internal variables when fire mode is updated (manual vs automatic)
  if (request->hasParam("fire_mode")){
    request->getParam("fire_mode")->value().toCharArray(char_game_mode,2);
    game_mode = atoi(char_game_mode);
    Serial.println(game_mode);
    request->send(SPIFFS,"/fireplace.html");  
  }
  // Make updates to internal variables when fire stoke button is pressed  
  if (request->hasParam("StokeFire")) {
    fire_stoked = 1;
    Serial.println("stoked");
    request->send(SPIFFS,"/fireplace.html");
  }

  // Make updates to internal variables when new image data is uploaded (using manual draw)
  if (request->hasParam("ImageData")){
    File imageFile = SPIFFS.open(image_filename, "w");
    imageFile.print(request->getParam("ImageData")->value());
    imageFile.close();
    updateImage = 1;
    request->send(SPIFFS,"/imagedisplay.html");
  }
  // Make updates to internal variables when new image data is uploaded (using image upload)  
  if (request->hasParam("ImageData-Upload")){
    File imageFile = SPIFFS.open(image_filename, "w");
    imageFile.print(request->getParam("ImageData-Upload")->value());
    imageFile.close();
    updateImage = 1;
    request->send(SPIFFS,"/imagedisplay.html");
  }
  
  });


  // Give 404 error for any requests that cannot be found  
  server.onNotFound(handleNotFound);

  // Actually begin the server now (after all of that is setup)
  server.begin();
  Serial.println("HTTP server started");
  
}
