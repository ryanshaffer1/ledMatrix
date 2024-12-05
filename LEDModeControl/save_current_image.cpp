// ---------------- Include other necessary files ----------------
// Include paired header file
#include "save_current_image.h"

// Built-in/installed Arduino libraries used in this file (other libraries may be used in other files)
#include "Arduino.h"
#include <SPIFFS.h>

// ---------------- Reference global variables declared elsewhere ----------------
extern String save_image_filename;
extern String image_filename;
extern String saved_images_list_filename;

// ---------------- Define variables used in the function below ----------------
char save_file_buf;
char read_file_buf;

// ---------------- Function ----------------
void save_current_image() {

  // Check that the file system can be opened (and open it in the process)
  if(!SPIFFS.begin(true)){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  }
  // Read current image file from SPIFFS
  File current_file = SPIFFS.open(image_filename);
  if(!current_file){
      Serial.println("Failed to open current file for reading");
      return;
  }
  // Open the save file for writing
  File write_file = SPIFFS.open("/saved_images/"+save_image_filename+".txt", "w");
  if(!write_file){
      Serial.println("Failed to open write file for writing");
      return;
  }

  // Write the current image data to the save file
  while(current_file.available()){
    save_file_buf = current_file.read();
    write_file.print(save_file_buf);
  }
  
  // Close both files
  current_file.close();
  write_file.close();



  // Add filename to list of saved images
  // Open saved image list file from SPIFFS
  File list_file = SPIFFS.open(saved_images_list_filename, FILE_APPEND);
  if(!list_file){
      Serial.println("Failed to open list file for reading");
      return;
  }
  // Write the current image name to the saved images list file
  Serial.println(save_image_filename);
  list_file.println(save_image_filename);
  
  // Close file
  list_file.close(); 
  
}
