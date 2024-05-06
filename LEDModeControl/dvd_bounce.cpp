// ---------------- Include other necessary files ----------------
// Include paired header file
#include "dvd_bounce.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <FastLED_NeoMatrix.h>

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;
extern uint16_t colors[];
extern uint16_t secondary_colors[];
extern uint16_t tertiary_colors[];
extern bool dvd_pattern;
extern bool dvd_random;


// ---------------- Function ----------------
void dvd_bounce() {
  
  // ---------------- SETUP ----------------

  // Manage display mode
  extern int display_mode_int;
  int og_display_mode = display_mode_int; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Display settings
  matrix->setBrightness(HIGH_BRIGHTNESS);

  // Set up initial state of "DVD Logo"
  int num_colors = 4; //Length of array "colors" (want to figure out how not to hard-code this)
  float x = mw/2;
  float y = mh/2-1;
  int round_x;
  int round_y;
  float dir_x = 1.5;
  float dir_y = 1;
  int color_index = 0;
  bool color_gen = 0;

  // Initial colors for the bouncing pixels
  uint16_t primary_color, secondary_color, tertiary_color;
  if (dvd_random){
    primary_color = matrix->Color(255,255,255);
    secondary_color = matrix->Color(127,127,127);
    tertiary_color = matrix->Color(69,69,69);
    srand(18952); // Seed color randomization
  }
  else{
    primary_color = colors[0];
    secondary_color = secondary_colors[0];
    tertiary_color = tertiary_colors[0];
  }

  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes) 
  while(1){

    // ---------------- Move logo ----------------
    
    // Increment x and y
    x = x + dir_x;
    y = y + dir_y;

    //Round x and y to the nearest integers for drawing
    round_x = round(x);
    round_y = round(y);
    
    // If (rounded) x or y are outside of the matrix bounds, replace their value with the nearest bound
    round_x = min(max(round_x,0),mw-1);
    round_y = min(max(round_y,0),mh-1);

    // Check for collisions against any of the matrix walls. Reverse x and/or y direction, and change to the next color in the color array
    int bump_offset = 0;
    if (dvd_pattern) bump_offset = 1;
    if ((round_x+bump_offset==mw-1)||(round_x-bump_offset==0)){
      dir_x = dir_x*(-1);
      color_gen = 1;
    }   
    if ((round_y+bump_offset==mh-1)||(round_y-bump_offset==0)){
      dir_y = dir_y*(-1);
      color_gen = 1;
    }

    // Generate new colors if the flag was set (
    if (color_gen){
      if (dvd_random){
        primary_color = matrix->Color(rand()%255,rand()%255,rand()%255);
        secondary_color = matrix->Color(rand()%255,rand()%255,rand()%255);
        tertiary_color = matrix->Color(rand()%255,rand()%255,rand()%255);      
        color_gen = 0; // Reset color change flag
      }
      else{
        color_index = (color_index+1)%num_colors;
        primary_color = colors[color_index];
        secondary_color = secondary_colors[color_index];
        tertiary_color = tertiary_colors[color_index];
        color_gen = 0; // Reset color change flag
      }
    }

    // ---------------- DISPLAY ----------------
    
    matrix->fillScreen(LED_BLACK); // Erase previous pixels

    //Draw center pixel on rounded x and y
    matrix->drawPixel(round_x,round_y,primary_color);

    //Draw surrounding pixel pattern around center (if flag is set)
    if (dvd_pattern){
      matrix->drawPixel(round_x+1,round_y+1,secondary_color);
      matrix->drawPixel(round_x+1,round_y-1,secondary_color);
      matrix->drawPixel(round_x-1,round_y+1,secondary_color);
      matrix->drawPixel(round_x-1,round_y-1,secondary_color);
      matrix->drawPixel(round_x+1,round_y,tertiary_color);
      matrix->drawPixel(round_x-1,round_y,tertiary_color);
      matrix->drawPixel(round_x,round_y+1,tertiary_color);
      matrix->drawPixel(round_x,round_y-1,tertiary_color);
    }

    // Push new pixel updates to the screen
    matrix->show();



    // ---------------- MODE MAINTENANCE/EVENT HANDLING ----------------
    // If display mode is different from the og value or mode button is pressed, end the loop
    if (display_mode_int != og_display_mode){
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;      
    }
    else if (digitalRead(BUTTONPIN)) {
      display_mode_int++;
      while(digitalRead(BUTTONPIN)){}
      delay(100);
      matrix->fillScreen(LED_BLACK);
      matrix->show();
      return;
    }
    
    // Delay before next frame
    delay(200);

  }

}
