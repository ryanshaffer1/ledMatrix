// ---------------- Include other necessary files ----------------
// Include paired header file
#include "display_image.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <FastLED_NeoMatrix.h>
#include <SPIFFS.h>

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// Don't know what this does, but let's do it anyway
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;
extern String image_filename;
extern bool updateImage;

// ---------------- Declare variables for functions below ----------------
char buf[8];
unsigned short myint;
extern unsigned short bitmapImage[1024];
int charsRead = 0;

// ---------------- Helper Function fixdrawRGBBitmap ----------------
// Does some reformatting of data, then draws a bitmap to the LEDs using the FASTLED_NeoMatrix library function(s)
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    // work around "a15 cannot be used in asm here" compiler bug when using an array on ESP8266
    // uint16_t RGB_bmp_fixed[w * h];
    static uint16_t *RGB_bmp_fixed = (uint16_t *) malloc( w*h*2);
    for (uint16_t pixel=0; pixel<w*h; pixel++) {
      uint8_t r,g,b;
      uint16_t color = pgm_read_word(bitmap + pixel);

      b = (color & 0xF00) >> 8;
      g = (color & 0x0F0) >> 4;
      r = color & 0x00F;
    
      // expand from 4/4/4 bits per color to 5/6/5
      b = map(b, 0, 15, 0, 31);
      g = map(g, 0, 15, 0, 63);
      r = map(r, 0, 15, 0, 31);
    
      RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
    }
    matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}

// ---------------- Function ----------------
void display_image() {
  // ---------------- SETUP ----------------
  
  // Manage display mode
  extern int display_mode;
  int og_display_mode = display_mode; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)
  while(1){

    // ---------------- Update Image ----------------
    if (updateImage){
      // Clear out image data
      charsRead = 0;
      for (int idx=0; idx<1024; idx++){
        bitmapImage[idx] = 0;
      }
      // Read file from SPIFFS
      if(!SPIFFS.begin(true)){
           Serial.println("An Error has occurred while mounting SPIFFS");
           return;
      }
      File file = SPIFFS.open(image_filename);
      if(!file){
          Serial.println("Failed to open file for reading");
          return;
      }
      while(file.available()){
        file.readBytesUntil('\n',buf,7);
        sscanf(buf, "%x", &myint);
        bitmapImage[charsRead] = myint;
        charsRead++;
      }
      file.close();
      updateImage = 0;
    }
  
    // ---------------- DISPLAY ----------------
    // Configure display
    matrix->setBrightness(LOW_BRIGHTNESS);
    matrix->fillScreen(LED_BLACK);
    
    // Draw image, using helper function above
    matrix->drawRGBBitmap(0, 0, (const uint16_t *) bitmapImage, mw, mh);
    
    matrix->show();
    
    // ---------------- MODE MAINTENANCE/EVENT HANDLING ----------------    
    // If display mode is different from the og value or mode button is pressed, end the loop
    if (display_mode != og_display_mode){
      delay(300);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;      
    }
    else if (digitalRead(BUTTONPIN)) {
      display_mode++;
      while(digitalRead(BUTTONPIN)){}
      delay(300);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;
    }
    // Delay before next "frame"
    delay(200);

  }

}
