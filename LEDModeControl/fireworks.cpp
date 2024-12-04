// ---------------- Include other necessary files ----------------
// Include paired header file
#include "fireworks.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <FastLED_NeoMatrix.h>
#include "FastLED.h"

// Other headers within the project that are used in this file
#include "matrix_declarations.h"



// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;
extern CRGB leds[NUMMATRIX];

// ---------------- New global variables used in this file ----------------
const int num_Fireworks = 10; // Maximum number of fireworks in use at a time. Used to allocate sizes of arrays

const int maxFragments = 12; // Maximum number of "fragments" on each firework (actual number may be lower). Used to allocate sizes of arrays



// ---------------- Define classes for use in function below ----------------
class Particle {
  public:
    float x;
    float y;
    float vx;
    float vy;
    bool explodes;
    bool active;
    Particle(){
      x = -1;
      y = -1;
      vx = 0;
      vy = 0;
      explodes = false;
      active = false;
    }
    Particle(float param1, float param2, float param3, float param4, bool param6, bool param7){
      x = param1;
      y = param2;
      vx = param3;
      vy = param4;
      explodes = param6;
      active = param7;
    }

    void updateParticle(float gravity){
      vy = vy + gravity;
      x = x + vx;
      y = y + vy;
    }

    void checkActive(){
      if(x<-1 || x>mw+1 || y>mh+1) active = false;
      // Giving one pixel of buffer so that random pixels don't get stuck on the border of the screen (happens sometimes during non-streamer fireworks)
    }

    void checkFragRadius(Particle core, float max_radius){
      float xdiff = x - core.x;
      float ydiff = y - core.y;
      if(xdiff*xdiff + ydiff*ydiff >= max_radius*max_radius){
        active = false;
      }
    }

    Particle createFragment(float theta, float exitSpeed){
      float frag_vx = vx + exitSpeed*cos(theta);
      float frag_vy = vy + exitSpeed*sin(theta);
      Particle fragment(x,y,frag_vx,frag_vy,false,true);
      return fragment;
    };
    
};

class Firework {
  public:
    int delay_time;
    float launch_x;
    float launch_y;
    float launch_vx;
    float launch_vy;
    int num_fragments;
    float frag_speed;
    float max_radius;
    uint16_t color;
    bool core_streamer;
    bool frag_streamer;
    // non-input properties
    bool active = false;
    Particle core;
    Particle fragments[maxFragments];

    Firework(){
      
    }
    Firework(int param1, float param2, float param3, float param4, float param5, int param6, float param7, float param8, uint16_t param9, bool param10, bool param11){
      delay_time = param1;
      launch_x = param2;
      launch_y = param3;
      launch_vx = param4;
      launch_vy = param5;
      num_fragments = param6;
      frag_speed = param7;
      max_radius = param8;
      color = param9;
      core_streamer = param10;
      frag_streamer = param11;

    }

    Particle createCore(){
      Particle core(launch_x,launch_y,launch_vx,launch_vy,true,active);
      return core;
    }

    int** updateFirework(float gravity, int** last_pos){
      // Update position of firework core
      // Store last position (prior to update) of core
      last_pos[0][0] = round(core.x);
      last_pos[0][1] = round(core.y);
      if (core.active) {
        core.updateParticle(gravity);
        core.checkActive();
      }

      // Update position of firework fragments
      for (int i = 0; i < num_fragments; i++){
        // Store last position (prior to update) for each fragment
        last_pos[i+1][0] = round(fragments[i].x);
        last_pos[i+1][1] = round(fragments[i].y);
        
        // Update position of active fragments
        if (fragments[i].active){
          fragments[i].updateParticle(gravity);
          fragments[i].checkActive(); // Check whether to make fragments outside of screen inactive (and stop updating/displaying)
          fragments[i].checkFragRadius(core,max_radius); // Check whether fragment has met/exceeded the maximum radius
          
          // If current position is same pixel as last position, clear last position from array so that it is not erased
          if (last_pos[i+1][0] == round(fragments[i].x) && last_pos[i+1][1] == round(fragments[i].y)){
            last_pos[i+1][0] = -1;
            last_pos[i+1][1] = -1;
          }
        }
      }
      return last_pos;
    }

    void explodeFirework(float gravity){
      // Create fragments (and remove core) when core reaches the apex of its trajectory
      if(core.explodes && (abs(core.vy)<gravity)){
        core.explodes = false;
        core.active = false;
        for (int i = 0; i < num_fragments; i++){
          float theta = i*2*3.14/num_fragments;
          fragments[i] = core.createFragment(theta,frag_speed);
        }
      }
    }

    void drawFirework(){
      // Draw core
      if (core.active){
        matrix->drawPixel(round(core.x), round(core.y), matrix->Color(255,255,255));
      }
      
      // Draw fragments
      for (int i = 0; i < num_fragments; i++){
        if (fragments[i].active){
          matrix->drawPixel(round(fragments[i].x), round(fragments[i].y), color);
        }
      }
    }

    void endFirework(){
      // If the core and fragments of the firework are inactive, the firework is inactive
      if (core.active) return;
      for (int i = 0; i < num_fragments; i++){
        if (fragments[i].active) return;
      }
      active = false;
      return;
    }
    
};






// ---------------- Function ----------------
void fireworks() {
  // ---------------- SETUP ----------------

  // Manage display mode
  extern int display_mode_int;
  int og_display_mode = display_mode_int; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Display settings
  matrix->setBrightness(LOW_BRIGHTNESS);





  

  // ---------------- Define variables for use in function below ----------------
  int SpeedDelay = 0;
  float gravity = 0.025;

  // Firework objects
//  const int num_Fireworks = 2;
//  Firework fw1(0,16,mh,0,-1,12,5,matrix->Color(0,0,255),false,true);
//  Firework fw2(0,2,mh,0.25,-1.2,48,1000,matrix->Color(255,0,0),false,false);
//  Firework fireworks[num_Fireworks] = {fw1,fw2};


  uint16_t red = matrix->Color(255,0,0);
  uint16_t white = matrix->Color(255,255,255);
  uint16_t blue = matrix->Color(0,0,255);
  
  Firework fw_bg1( 0, 7,mh, 0.000,-1.200,12,1.0,10, blue,false, true); // Blue background
  Firework fw_st1(10, 5,mh,-0.050,-1.200,12,0.5, 2,white,false, true); // Star
  Firework fw_st2(10, 5,mh, 0.110,-1.200,12,0.5, 2,white,false, true); // Star
  Firework fw_st3(10, 5,mh, 0.050,-1.050,12,0.5, 2,white,false, true); // Star
  Firework fw_r11( 0,16,mh, 0.000,-1.250,12,0.5, 3,  red,false, true); // Red Stripe 1
  Firework fw_r12( 0,16,mh, 0.130,-1.250,12,0.5, 3,  red,false, true); // Red Stripe 1
  Firework fw_r13( 0,17,mh, 0.250,-1.250,12,0.5, 3,  red,false, true); // Red Stripe 1
  Firework fw_w11( 0,16,mh, 0.000,-1.110,12,0.5, 3,white,false, true); // White Stripe 1
  Firework fw_w12( 0,16,mh, 0.130,-1.110,12,0.5, 3,white,false, true); // White Stripe 1
  Firework fw_w13( 0,17,mh, 0.250,-1.110,12,0.5, 3,white,false, true); // White Stripe 1
//  Firework fw_r21( 0,16,mh, 0.000,-1.050,12,0.5, 3,  red, true, true); // Red Stripe 2
//  Firework fw_r22( 0,16,mh, 0.130,-1.050,12,0.5, 3,  red, true, true); // Red Stripe 2
//  Firework fw_r23( 0,17,mh, 0.250,-1.050,12,0.5, 3,  red, true, true); // Red Stripe 2
//  Firework fw_w21( 0,15,mh,-0.275,-1.000,12,0.5, 3,white, true, true); // White Stripe 2
//  Firework fw_w22( 0,16,mh,-0.130,-1.000,12,0.5, 3,white, true, true); // White Stripe 2
//  Firework fw_w23( 0,16,mh, 0.000,-1.000,12,0.5, 3,white, true, true); // White Stripe 2
//  Firework fw_w24( 0,16,mh, 0.130,-1.000,12,0.5, 3,white, true, true); // White Stripe 2
//  Firework fw_w25( 0,17,mh, 0.275,-1.000,12,0.5, 3,white, true, true); // White Stripe 2  
//  Firework fw_r31( 0,15,mh,-0.275,-0.800,12,0.5, 3,  red, true, true); // Red Stripe 3
//  Firework fw_r32( 0,16,mh,-0.130,-0.800,12,0.5, 3,  red, true, true); // Red Stripe 3
//  Firework fw_r33( 0,16,mh, 0.000,-0.800,12,0.5, 3,  red, true, true); // Red Stripe 3
//  Firework fw_r34( 0,16,mh, 0.130,-0.800,12,0.5, 3,  red, true, true); // Red Stripe 3
//  Firework fw_r35( 0,17,mh, 0.275,-0.800,12,0.5, 3,  red, true, true); // Red Stripe 3
//  Firework fireworks[num_Fireworks] = {fw_bg1,fw_st1,fw_st2,fw_st3,fw_r11,fw_r12,fw_r13,fw_w11,fw_w12,fw_w13,fw_r21,fw_r22,fw_r23,fw_w21,fw_w22,fw_w23,fw_w24,fw_w25,fw_r31,fw_r32,fw_r33,fw_r34,fw_r35};
  Firework fireworks[num_Fireworks] = {fw_bg1,fw_st1,fw_st2,fw_st3,fw_r11,fw_r12,fw_r13,fw_w11,fw_w12,fw_w13};


  
  for (int i = 0; i < num_Fireworks; i++){
    fireworks[i].core = fireworks[i].createCore();
  }

  // 3D Array to hold the previous position of each fragment (used for non-streamer fireworks)
  int*** last_pos = new int**[num_Fireworks];
  for (int i = 0; i < num_Fireworks; i++){
    last_pos[i] = new int*[maxFragments+1]; 
    for (int j = 0; j < maxFragments+1; j++){
        last_pos[i][j] = new int[2];
    }    
  }



  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)   
  int animation_cycle = 0;
  // Keep track of the current firework
  int fw_count = 0;
  
  while(1){

    // UPDATE ALL FIREWORKS
    for (int i = 0; i < num_Fireworks; i++){
      // Step 0. Make fireworks active based on delay time
      if (fireworks[i].delay_time == animation_cycle){
        fireworks[i].active = true;
        fireworks[i].core.active = true;
      }
      
      // Step 1. Update particles' positions and velocities 
      last_pos[i] = fireworks[i].updateFirework(gravity,last_pos[i]);
  
      // Step 2. Explode fireworks that reach their apex    
      fireworks[i].explodeFirework(gravity);
  
      // Step 3. If firework is no longer onscreen, make it inactive
      fireworks[i].endFirework();

    }

    

    // ---------------- DISPLAY ----------------
//    matrix->fillScreen(0);

    // Slowly fade every pixel to black
    fadeToBlackBy(leds,NUMMATRIX,10);

    // Display all fireworks
    for (int i = 0; i < num_Fireworks; i++){
      fireworks[i].drawFirework();
      // Black out the last_pos for fireworks with streaming set to false
      if (!fireworks[i].core_streamer){
        matrix->drawPixel(last_pos[i][0][0], last_pos[i][0][1], matrix->Color(0,0,0));
      }
      if (!fireworks[i].frag_streamer){
        for (int j = 1; j < fireworks[i].num_fragments+1; j++){
          matrix->drawPixel(last_pos[i][j][0], last_pos[i][j][1], matrix->Color(0,0,0));
        }
      }    
    }

    // Push display to LEDs
    matrix->show(); // Show all display updates

    // Increase animation iteration counter
    animation_cycle++;

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
