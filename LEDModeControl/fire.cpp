// ---------------- Include other necessary files ----------------
// Include paired header file
#include "fire.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <FastLED_NeoMatrix.h>

// Other headers within the project that are used in this file
#include "matrix_declarations.h"



// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;
extern bool game_mode;
extern bool fire_stoked; // Will be set to 1 if handle_wifi detects that the user pressed the stoke fire button

// ---------------- Function ----------------
void fire() {
  // Heavily modified from: https://forum.arduino.cc/t/serial-begin-9600-prevents-my-led-script-from-running/571129
  // ---------------- SETUP ----------------

  // Manage display mode
  extern int display_mode_int;
  int og_display_mode = display_mode_int; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Display settings
  matrix->setBrightness(LOW_BRIGHTNESS);

  // ---------------- Define variables for use in function below ----------------
  // Heat energy transfer parameters 
  int Cooling = 155; // Higher number = more cooling (at all parts of the fire)
  int Sparking = 160; // Higher number = more sparks (at the bottom of the fire)
  int bottom_side_spread = 6; // How much the bottom row spreads heat. Higher number = less side spread (center cell is more heavily weighted)
  int up_diag_spread = 10; // How much heat spreads diagonally upwards. Higher number = less diagonal spread (center cell is more heavily weighted)
  double fire_heat_loss = 0.97; // Efficiency of heat transfer upwards. Higher = more efficient heat transfer (so higher flames)
  // Ember parameters (note that embers are purely visual)
  int Embers = 50;  
  bool current_ember = 0;
  int ember_x = 0;
  int ember_y = 0;
  int ember_heat = 0;
  int ember_step = 0;
  int ember_rise_steps = 4;
  // Animation/game parameters
  int num_initial_sparks = 10;
  int SpeedDelay = 0;
  int game_cycle_count = 500;
  double og_heat_loss = fire_heat_loss; // Track original value
  double min_heat_loss = 0.8;
  // Declare a few more variables used within the function
  static byte heat[NUMMATRIX];
  int cooldown;

  
  // Start the fire off with many sparks in the middle
  for ( int i = 0; i < num_initial_sparks; i++){
    int x = round(random(mw/3,2*mw/3));
    int spark_index = mw*(mh-1) + x;
    heat[spark_index] = 255;
    heat[spark_index-1] = 255/3;
    heat[spark_index+1] = 255/3;
  }    


  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)   
  int animation_cycle = 0;
  while(1){

    // Step 0. Increase iteration counter
    animation_cycle++;
    
    // Step 1.  Cool down every cell (except for embers) a little
    for( int i = 0; i < NUMMATRIX; i++) {
      cooldown = random(0, ((Cooling * 10) / mh) + 2);
      if(cooldown>heat[i]) {
        heat[i]=0; // Ensure heat is >= 0
      } else {
        heat[i]=heat[i]-cooldown;
      }
    }

    // Step 2a. Heat spreads a little to the left and right along the bottom (with no loss) (heavily weighting the cell's own heat in it's calculation)
    for( int k= mh-1; k >= mh-3; k--) {
      for (int j = 0; j < mw; j++){
        int index = mw*k + j;
          if (j >= mw-1) heat[index] = (bottom_side_spread*heat[mw*(k) + j] + heat[mw*(k) + j-1] + heat[mw*(k) + j-2]) / (bottom_side_spread+2);
          else if (j <= 1) heat[index] = (bottom_side_spread*heat[mw*(k) + j] + heat[mw*(k) + j+1] + heat[mw*(k) + j+2]) / (bottom_side_spread+2);
          else heat[index] = (bottom_side_spread*heat[mw*(k) + j] + heat[mw*(k) + j+1] + heat[mw*(k) + j-1]) / (bottom_side_spread+2);
      }
    }
    
    // Step 2b.  Heat from each cell drifts 'up' (plus a little on the diagonal) and diffuses a little  
    for( int k= mh - 1; k >= 2; k--) {
      for (int j = 0; j < mw; j++){
        int index = mw*k + j;
          if (j >= mw-1) heat[index] = (up_diag_spread*heat[mw*(k+1) + j] + heat[mw*(k+1) + j-1] + heat[mw*(k+1) + j-2]) / (up_diag_spread+2) * fire_heat_loss;
          else if (j <= 1) heat[index] = (up_diag_spread*heat[mw*(k+1) + j] + heat[mw*(k+1) + j+1] + heat[mw*(k+1) + j+2]) / (up_diag_spread+2) * fire_heat_loss;
          else heat[index] = (up_diag_spread*heat[mw*(k+1) + j] + heat[mw*(k+1) + j+1] + heat[mw*(k+1) + j-1]) / (up_diag_spread+2) * fire_heat_loss;
      }
    }
      
    // Step 3.  Randomly ignite new 'sparks' near the bottom
    int scaledSparking = round(Sparking * pow((fire_heat_loss/og_heat_loss),2)); // Sparking scaled by the change in loss factor (so that as the game progress and the fire dies down, sparks also slow)
    if( random(255) < scaledSparking) { 
      int y = round(random(3));
      int x = round(random(2,mw-2));
      int spark_index = mw*(mh-y) + x;
      heat[spark_index] = random(255*2/3,255);
    }

    // Step 4.  Embers drifting to the top
    if( current_ember){
      ember_step++;
      if (ember_step == ember_rise_steps) {
        ember_y--;
        ember_step = 0;
      }
      if( ember_y < 0) current_ember = 0;
    }
    else{
      if( random(255) < Embers) {
        current_ember = 1;
        ember_x = round(random(2,mw-2));
        // Set ember_y as the highest point on the "column" of ember_x with some heat
        ember_y = 0;
        while( heat[mw*ember_y + ember_x] < 255/3 && ember_y<mh) ember_y++;
        ember_heat = heat[mw*ember_y + ember_x];
      }  
    }

    // Step 5a.  If in game mode, decrease the loss factor over time until hitting the minimum
    if (animation_cycle >= game_cycle_count) {
      if (game_mode) {
        fire_heat_loss = max(fire_heat_loss - 0.005,min_heat_loss);
        Serial.println(fire_heat_loss);
      }
      animation_cycle = 0;
    }
    // Step 5b.  If in game mode, increase the loss factor upon receiving user input
    if (fire_stoked){
      fire_heat_loss = min(fire_heat_loss + 0.03,og_heat_loss); // Increase by a small amount until reaching the original
      animation_cycle = 0; // Reset countdown to the next decrease
      Serial.println(fire_heat_loss);
      fire_stoked = 0; // Reset
    }

    // Step 5c.  If not in game mode, keep loss factor the same as the original value
    if (!game_mode) fire_heat_loss = og_heat_loss;


    // ---------------- DISPLAY ----------------
 
    // Step 6.  Convert heat to LED colors
    for( int i = 0; i < NUMMATRIX; i++) {
        int x = i%mw;
        int y = floor(i/mw);
        if (heat[i] > 255*2/3){
          matrix->drawPixel(x,y,matrix->Color(heat[i], heat[i], 0));
        }
        else if (heat[i] > 255*1/4){
          matrix->drawPixel(x,y,matrix->Color(heat[i], heat[i]/2, 0));
        }
        else{
          matrix->drawPixel(x,y,matrix->Color(heat[i], 0, 0));
        }
    }
    // Step 5d. Add ember heat at the end (to prevent fading/diffusion)
    if( current_ember) matrix->drawPixel(ember_x,ember_y,matrix->Color(ember_heat,0,0));
  
    matrix->show(); // Show all display updates

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
     // Delay before next cycle
    delay(SpeedDelay);
  }
}
