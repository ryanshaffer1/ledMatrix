// ---------------- Include other necessary files ----------------
// Include paired header file
#include "reset_default_image.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <SPIFFS.h>

// ---------------- Reference global variables declared elsewhere ----------------
extern String default_image_filename;
extern String image_filename;

// ---------------- Define variables used in the function below ----------------
char reset_file_buf;

// ---------------- Function ----------------
void reset_default_image() {

  // Check that the file system can be opened (and open it in the process)
  if(!SPIFFS.begin(true)){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  }
  // Read default file from SPIFFS
  File default_file = SPIFFS.open(default_image_filename);
  if(!default_file){
      Serial.println("Failed to open file for reading");
      return;
  }
  // Open the image file for writing
  File write_file = SPIFFS.open(image_filename, "w");
  if(!write_file){
      Serial.println("Failed to open file for reading");
      return;
  }

  // Write the default image data to the open image file
  while(default_file.available()){
    reset_file_buf = default_file.read();
    write_file.print(reset_file_buf);
  }

  // Close both files
  default_file.close();
  write_file.close();


}
