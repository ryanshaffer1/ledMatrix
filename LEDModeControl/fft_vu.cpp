// ---------------- Include other necessary files ----------------
// Include paired header file
#include "fft_vu.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include <Adafruit_GFX.h>
#include <arduinoFFT.h>
#include <FastLED_NeoMatrix.h>

// Other headers within the project that are used in this file
#include "matrix_declarations.h"

// ---------------- Reference global variables declared elsewhere ----------------
extern FastLED_NeoMatrix *matrix;

// ---------------- Declare variables for functions below ----------------
// Not great practice to use #define, but I stole this code from the internet and it works, so oh well
#define SAMPLES         512          // Must be a power of 2
#define SAMPLING_FREQ   10000         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE       10        // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
const int BRIGHTNESS_SETTINGS[3] = {25, 70, 200};  // 3 Integer array for 3 brightness settings (based on pressing+holding BTN_PIN)
#define NUM_BANDS       16            // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE           1000           // Used as a crude noise filter, values below this are ignored
#define BAR_WIDTH      (mw  / (NUM_BANDS - 1))  // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc
#define TOP            (mh - 0)                // Don't allow the bars to go offscreen
// Initialize large arrays
double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// ---------------- Function ----------------
void fft_vu() {
  // ---------------- SETUP ----------------

  // Manage display mode
  extern int display_mode_int;
  int og_display_mode = display_mode_int; // Keep track of what the mode number was that entered this mode (instead of hard-coding an int)

  // Set up sampling and FFT stuff
  unsigned int sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
  byte peak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};              // The length of these arrays must be >= NUM_BANDS
  int oldBarHeights[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int bandValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned long newTime;

  // ---------------- LOOP ----------------
  // Loop is while(1), meaning it will run continuously until broken (break the loop by changing modes)
  while(1){

    // ---------------- Sample data from Audio Pin ----------------
    
    // Reset bandValues[]
    for (int i = 0; i<NUM_BANDS; i++){
      bandValues[i] = 0;
    }
    
    // Sample the audio pin
    for (int i = 0; i < SAMPLES; i++) {
      newTime = micros();
      vReal[i] = analogRead(AUDIOPIN); // A conversion takes about 9.7uS on an ESP32
      vImag[i] = 0;
      while ((micros() - newTime) < sampling_period_us) { /* chill */ }
    }

    // ---------------- Process data using FFT (Fast Fourier Transform) ----------------
    
    // Compute FFT
    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
  
    // Analyse FFT results
    for (int i = 2; i < (SAMPLES/2); i++){       // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
      if (vReal[i] > NOISE) {                    // Add a crude noise filter
  
      //16 bands, 4.5kHz top band
//      if (i<=11 )           bandValues[0]  += (int)vReal[i];
      if (i>11   && i<=14  ) bandValues[1]  += (int)vReal[i];
      if (i>14   && i<=17  ) bandValues[2]  += (int)vReal[i];
      if (i>17   && i<=21  ) bandValues[3]  += (int)vReal[i];
      if (i>21   && i<=26  ) bandValues[4]  += (int)vReal[i];
      if (i>26   && i<=32  ) bandValues[5]  += (int)vReal[i];
      if (i>32   && i<=40  ) bandValues[6]  += (int)vReal[i];
      if (i>40   && i<=49  ) bandValues[7]  += (int)vReal[i];
      if (i>49   && i<=60  ) bandValues[8]  += (int)vReal[i];
      if (i>60   && i<=74  ) bandValues[9]  += (int)vReal[i];
      if (i>74   && i<=91  ) bandValues[10]  += (int)vReal[i];
      if (i>91   && i<=112  ) bandValues[11]  += (int)vReal[i];
      if (i>112   && i<=138  ) bandValues[12]  += (int)vReal[i];
      if (i>138   && i<=170  ) bandValues[13]  += (int)vReal[i];
      if (i>170   && i<=209  ) bandValues[14]  += (int)vReal[i];
      if (i>209             ) bandValues[15]  += (int)vReal[i];

      }
    }

    // ---------------- DISPLAY ----------------
    FastLED.clear(); // Clear display

    // Post-process and display each sound bar one at a time
    
    // Process the FFT data into bar heights
    for (byte band = 0; band < NUM_BANDS; band++) {
  
      // Scale the bars for the display
      int barHeight = bandValues[band] / AMPLITUDE;
      if (barHeight > TOP) barHeight = TOP;
  
      // Small amount of averaging between frames
      barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;
  
      // Move peak up
      if (barHeight > peak[band]) {
        peak[band] = min(TOP, barHeight);
      }

      // Draw rainbow bars
      int xStart = BAR_WIDTH * band;
      for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
        for (int y = TOP; y >= TOP - barHeight; y--) {
          matrix->drawPixel(x, y, CHSV((x / BAR_WIDTH) * (255 / NUM_BANDS), 255, 255));
        }
      }
      // Draw white peaks
      int peakHeight = TOP - peak[band] - 1;
      for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
        matrix->drawPixel(x, peakHeight, CHSV(0,0,255));
      }
      
      // Save oldBarHeights for averaging later
      oldBarHeights[band] = barHeight;
    }
  
    // Decay peak
    EVERY_N_MILLISECONDS(60) {
      for (byte band = 0; band < NUM_BANDS; band++)
        if (peak[band] > 0) peak[band] -= 1;
//      colorTimer++;
    }
    FastLED.show(); // Show all display updates


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
//    delay(100);
  }
  
}
