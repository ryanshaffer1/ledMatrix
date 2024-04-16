 
#ifndef usersettings_header
  #define usersettings_header

// ---------------- WiFi Credentials ----------------
  const char* ssid = "Lisan-Al-Gaib-5G";
  const char* password = "Rotundis";

// ---------------- Domain Name ----------------
  const char* domain_name = "ledControl";

// ---------------- API Keys ----------------
  // OpenWeather
  // Sign up for an account here: https://openweathermap.org/api. Then paste your API key below:
  String openWeatherMapApiKey = "863302a2bf74bd059f22b0d257d5098d";
  
  // WMATA (Washington Metro Area Transit Authority, aka DC Metro)
  // Sign up for an account here: https://developer.wmata.com/
  String wmataApiKey = "4dfc633551b9439594b4e08ded8a1716";


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
