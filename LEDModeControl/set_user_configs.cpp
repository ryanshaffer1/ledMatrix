// ---------------- Include other necessary files ----------------
// Include paired header file
#include "set_user_configs.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <SPIFFS.h>

// ---------------- Reference global variables declared elsewhere ----------------
extern int num_display_modes;
extern char display_modes[][6];
extern String user_config_filename;
extern String valid_display_modes_filename;

// ---------------- Declare variables used in the function below ----------------
String valid_file_buf;
String temp;
int tabIndex[4];
char curr_mode[6];
String mode_name;
int validModesCount = 0;
char validDisplayModes[20][6];
int numSeenModes = 0;
char seenUserModes[20][6];
bool validMode;

// ---------------- Function ----------------
void set_user_configs() {

  // Check that the file system can be opened (and open it in the process)
  if(!SPIFFS.begin(true)){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  }

  // Reads in list of valid display modes
  File valid_modes_file = SPIFFS.open(valid_display_modes_filename);
  if(!valid_modes_file){
      Serial.println("Failed to open file for reading");
      return;
  }
  valid_file_buf = valid_modes_file.readStringUntil('\n'); // Skip the first line of the file (lists out the column headers)
  while(valid_modes_file.available()){
    valid_file_buf = valid_modes_file.readStringUntil('\n'); // Obtain one row of the file
    // This row contains 4 tab ("\t") characters. Find the index of each:
    tabIndex[0] = valid_file_buf.indexOf("\t");
    temp = valid_file_buf.substring(tabIndex[0]+1);
    for (int i = 1; i < 4; i++){
      tabIndex[i] = temp.indexOf("\t") + tabIndex[i-1] + 1;
      temp = valid_file_buf.substring(tabIndex[i]+1);
    }
    mode_name = valid_file_buf.substring(tabIndex[0]+1,tabIndex[1]); // Get the current display mode 5-letter name as a String
    mode_name.toCharArray(validDisplayModes[validModesCount],6); // Append to the char array of valid display modes 5-letter names
    validModesCount++;
  }
    for (int j = 0; j < validModesCount;j++){
      Serial.println(validDisplayModes[j]);
    }

  // Validates user-selected list of display modes
  for(int i = 0; i < num_display_modes; i++){
    validMode = false;
    strcpy(curr_mode,display_modes[i]);
    // Check for presence of mode name in list of valid names
    for (int j = 0; j < sizeof(validDisplayModes)/sizeof(validDisplayModes[0]);j++){
      if(!strcmp(curr_mode,validDisplayModes[j])){
        validMode = true;
        break;
        }
      }
    if (!validMode) {
      Serial.printf("Invalid display mode listed in USER_SETTINGS.h: %s \n",curr_mode);
      strcpy(display_modes[i],"rsrvd"); // "rsrvd" used to denote that this line in display modes should be skipped (on the client side)
    }
    
    // Check for duplicates
    validMode = true;
    for (int j = 0; j < sizeof(seenUserModes)/sizeof(seenUserModes[0]);j++){
      if(!strcmp(curr_mode,seenUserModes[j])){
        validMode = false;
        break;
        }
      }
    if (!validMode) {
      Serial.printf("Duplicate display mode listed in USER_SETTINGS.h: %s \n",curr_mode);
      strcpy(display_modes[i],"rsrvd"); // "rsrvd" used to denote that this line in display modes should be skipped (on the client side)
    }
    else {
      strcpy(seenUserModes[numSeenModes],curr_mode);
      numSeenModes++;
    }   
  }

  // Open the user config file for writing
  File write_file = SPIFFS.open(user_config_filename, "w");
  if(!write_file){
      Serial.println("Failed to open file for writing");
      return;
  }


  // Write the user config data to the open file
//  Serial.println(display_modes);
  for(int i = 0; i < num_display_modes; i++){
    write_file.println(display_modes[i]);
  }

  // Close both files
  write_file.close();


}
