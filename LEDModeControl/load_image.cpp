// ---------------- Include other necessary files ----------------
// Include paired header file
#include "load_image.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <SPIFFS.h>

// ---------------- Reference global variables declared elsewhere ----------------
extern String load_image_filename;
extern String image_filename;

// ---------------- Define variables used in the function below ----------------
char load_file_buf;

// ---------------- Function ----------------
void load_image() {

  // Check that the file system can be opened (and open it in the process)
  if(!SPIFFS.begin(true)){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  }
  // Read default file from SPIFFS
  File load_file = SPIFFS.open(load_image_filename);
  if(!load_file){
      Serial.println("Failed to open load file for reading");
      return;
  }
  // Open the image file for writing
  File write_file = SPIFFS.open(image_filename, "w");
  if(!write_file){
      Serial.println("Failed to open read file for writing");
      return;
  }

  // Write the default image data to the open image file
  while(load_file.available()){
    load_file_buf = load_file.read();
    write_file.print(load_file_buf);
  }

  // Close both files
  load_file.close();
  write_file.close();

}
