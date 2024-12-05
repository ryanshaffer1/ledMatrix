// ---------------- Include other necessary files ----------------
// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Arduino.h>
//#include <WiFi.h>
//#include <ESPmDNS.h>
//#include <WiFiClient.h>
//#include <WebServer.h>
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
//#include <FastLED.h>
// Don't know what this does, but let's do it anyway
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// Include files for user settings/global definitions
#include "USER_SETTINGS.h"
#include "matrix_declarations.h"

// Include helper files
#include "handle_wifi.h"
#include "set_user_configs.h"
#include "save_current_image.h"
#include "load_image.h"

// Include display mode files
#include "clock_display.h"
#include "display_image.h"
#include "dvd_bounce.h"
#include "fire.h"
#include "fireworks.h"
#include "fft_vu.h"
#include "metro_tracker.h"
#include "plasma_gradient.h"
#include "scroll_text.h"

// ---------------- Reference global variables defined elsewhere -----------------------
extern int num_display_modes;
extern char display_modes[][6];
extern uint8_t text_size;

// ---------------- Declare global variables ----------------
// Define LED Matrix
CRGB leds[NUMMATRIX];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 16, 16, mw / 16, mh / 16,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
    NEO_TILE_BOTTOM + NEO_TILE_LEFT + NEO_TILE_COLUMNS + NEO_TILE_ZIGZAG);

// Define RGB colors
uint16_t colors[] = {matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255), matrix->Color(255, 255, 255) }; // Red, Green, Blue, White
uint16_t secondary_colors[] = {matrix->Color(255, 100, 0), matrix->Color(0, 255, 100), matrix->Color(200, 0, 255), matrix->Color(127, 127, 127) };
uint16_t tertiary_colors[] = {matrix->Color(255, 200, 0), matrix->Color(255, 255, 255), matrix->Color(255, 127, 127), matrix->Color(68, 68, 68) };
uint16_t textcolor = {matrix->Color(255, 255, 255)};

// Helper files in ESP32 memory
extern String default_image_filename;
String image_filename = "/image.txt";
String user_config_filename = "/userConfig.txt";
String valid_display_modes_filename = "/all_display_modes.txt";
String saved_images_list_filename = "/savedImages.txt";
String save_image_filename; // This is defined by the user
String load_image_filename = default_image_filename; // Start with default image

//Settings for the initial run case and various mode information
int display_mode_int = 0;
char display_mode_char[6];
// dvd_bounce parameters
bool dvd_random = 1;
bool dvd_pattern = 1;
// fireplace parameters
bool game_mode = 0;
bool fire_stoked = 0;
// image parameters
bool updateImage = 1;
unsigned short bitmapImage[1024];

// ---------------- SETUP ----------------
void setup() {
  // Start Serial for the ability to monitor data
  Serial.begin(115200);
  
  // Set default files and config files
  load_image();
  set_user_configs();
  
  
  //  Begin matrix, configure, and clear screen
  FastLED.addLeds<NEOPIXEL, PIN>(  leds, NUMMATRIX  ).setCorrection(TypicalLEDStrip);
  matrix->begin();
  matrix->setBrightness(LOW_BRIGHTNESS);
  matrix->setTextSize(text_size);
  matrix->fillScreen(LED_BLACK);
  matrix->show();

  // Begin WiFi handling
  handle_wifi();

}

// ---------------- LOOP ----------------
void loop() {
  // Loop is just a wrapper to manage the different display modes and run the function corresponding to the current display_mode
  if (display_mode_int == num_display_modes) display_mode_int = 0;
  strcpy(display_mode_char,display_modes[display_mode_int]); // Get name of current display mode

  if (!strcmp(display_mode_char,"clock")){
    clock_display();
  }
  else if (!strcmp(display_mode_char,"metro")){
    metro_tracker();
  }
  else if (!strcmp(display_mode_char,"text_")){
    scroll_text();
  }
  else if (!strcmp(display_mode_char,"firep")){
    fire();
  }
  else if (!strcmp(display_mode_char,"music")){
    fft_vu();
  }
  else if (!strcmp(display_mode_char,"image")){
    display_image();
  }
  else if (!strcmp(display_mode_char,"groov")){
    plasma_gradient();
  }
  else if (!strcmp(display_mode_char,"bounc")){
    dvd_bounce();
  }
  else if (!strcmp(display_mode_char,"firew")){
    fireworks();
  }

}
