 
#ifndef usersettings_header
  #define usersettings_header

// ---------------- WiFi Credentials ----------------
  const char* ssid = "XX_YOUR_NETWORK_NAME_XX";
  const char* password = "XX_YOUR_NETWORK_PASSWORD_XX";

// ---------------- Domain Name ----------------
  const char* domain_name = "ledControl";

// ---------------- API Keys ----------------
  // OpenWeather
  // Sign up for an account here: https://openweathermap.org/api. Then paste your API key below:
  String openWeatherMapApiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  
  // WMATA (Washington Metro Area Transit Authority, aka DC Metro)
  // Sign up for an account here: https://developer.wmata.com/
  String wmataApiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


// ---------------- API Settings ----------------
  // ntp Time Server Time Zone - offset in hours from GMT:
  int gmtOffset_hours = -5;

  // OpenWeather location
  String city = "Washington,DC";
  String countryCode = "USA";

  // WMATA default station code
  // List of station codes vs station names is here: 
  String StationCode = "C04"; // FOGGY BOTTOM


// ---------------- Image Information ----------------
  // Name of file to use as the default image in display_image mode:
  String default_image_filename = "/bliss.txt";
  // Note: must be saved in the "data" subfolder of this project (alongside "image.txt")


// ---------------- LED Mode-Specific Settings ----------------
  // scroll_text:
  String input_text = "Your Text Here"; // Default/initial text to display
  uint8_t text_size = 2; // Default/initial text size (may be 1, 2, or 4)
  
#endif
