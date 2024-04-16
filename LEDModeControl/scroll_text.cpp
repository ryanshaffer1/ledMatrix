// ---------------- Include other necessary files ----------------
// Include paired header file
#include "scroll_text.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// Don't know what this does, but let's do it anyway
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif


// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;
extern uint16_t colors[];
extern String input_text;
extern uint8_t text_size;

// ---------------- Function ----------------
void scroll_text() {
  // ---------------- Setup ----------------

  // Manage display mode
  extern int display_mode;
  int og_display_mode = display_mode; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Display settings
  matrix->setBrightness(HIGH_BRIGHTNESS);
  matrix->setTextColor(colors[0]);
  matrix->setTextWrap(false);
  matrix->setTextSize(text_size);

  // Define some variables
  int x    = mw;
  int pass = 0;

  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)
  while(1){
    
    // Compute the pixel width of the text
    int text_length = input_text.length();
    int pixel_width = text_length * 6 * text_size;

    // Move the "cursor"
    x--;
    if(x < -pixel_width) {
      x = mw;
      if(++pass >= 3) pass = 0;
      matrix->setTextColor(colors[pass]);
    }


    // ---------------- DISPLAY ----------------

    // Print text at point x
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    matrix->print(input_text);
    matrix->show();
    
    // ---------------- MODE MAINTENANCE/EVENT HANDLING ----------------
    // If display mode is different from the og value or mode button is pressed, end the loop
    if (display_mode != og_display_mode){
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;      
    }
    else if (digitalRead(BUTTONPIN)) {
      display_mode++;
      while(digitalRead(BUTTONPIN)){}
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;
    }

    // Delay before next frame
    delay(100);
  }
  
}
