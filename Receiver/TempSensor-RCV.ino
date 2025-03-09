/*
MIT License

Copyright (c) 2025 Carl Brothers

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights   
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      
copies of the Software, and to permit persons to whom the Software is         
furnished to do so, subject to the following conditions:                      

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.                               

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

/*
  Project Credits:
    - Author:  Carl Brothers
    - Email:   decibelkaos@gmail.com
    - Location: Not Seattle, Washington (But Close)
  
  Description:
    This project displays temperature/humidity data received via ESP-Now,
    stores configuration in NVS (Preferences), and serves a web page to
    configure settings and see real-time updates.

    Hardware/Library Overview:
      - Uses an SSD1306 OLED (via U8g2) to display data and animations.
      - Adafruit_NeoPixel library to drive a single RGB LED.
      - Wi-Fi in softAP mode for hosting a configuration web page.
      - JSON/ArduinoJson for sending and receiving config updates.
      - NVS (Preferences) for persisting user settings.
      - The file `webpage.h` holds the HTML/JS content served to the client.
*/

#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>    
#include "webpage.h"        // HTML/JS interface

// ----------------- NVS (Preferences) -----------------
Preferences preferences;

// ----------------- RGB LED Setup ---------------------
#define LED_PIN 48
#define NUM_LEDS 1
Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ----------------- OLED Setup ------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0, /* reset=*/ U8X8_PIN_NONE
);

// ----------------- Data Structure --------------------
typedef struct sensorData {
  float temperature; // Celsius
  float humidity;    // Percentage
} sensorData;

// Latest sensor data from ESP-Now
volatile sensorData latestData = {25.0, 50.0};

// ----------------- Global Configuration --------------
float marqueeLeftSpeed  = 1.3;
float marqueeRightSpeed = 1.0;
bool  marqueeEnabled    = true;
bool  scrollingEnabled  = true;
char  scrollingText[257] = "HUMIDITY AND TEMPERATURE DEFAULT";
unsigned long updateInterval = 20; // ms

// Display arrangement
char topPosition[10]    = "tempBoth";
char middlePosition[10] = "humidity";

// LED behavior
bool  ledEnabled          = true;
float humidityThresholdLow  = 33.3;
float humidityThresholdHigh = 66.6;
int   ledIntensity          = 100; // 0-255

// LED animation
char  ledAnimationMode[16]  = "static";
char  ledColorScheme[32]    = "Default";
unsigned long ledAnimSpeed  = 3000;

// ----------------- Color & ColorScheme Structures ----
struct Color {
  uint8_t r, g, b;
};

struct ColorScheme {
  const char* name;
  Color colors[7];  // Up to 7 colors for some schemes
  uint8_t count;
};

// A variety of color schemes. Feel free to add or modify.
ColorScheme colorSchemes[] = {
  { "Default",      { {255,0,0}, {0,255,0}, {0,0,255} }, 3 },
  { "Easter",       { {255,182,193}, {255,228,225}, {255,192,203} }, 3 },
  { "Christmas",    { {255,0,0}, {0,255,0}, {0,0,255} }, 3 },
  { "Halloween",    { {255,165,0}, {128,0,128}, {0,0,0} }, 3 },
  { "Valentine",    { {255,20,147}, {255,105,180}, {255,182,193} }, 3 },
  { "Independence", { {0,0,255}, {255,0,0}, {255,255,255} }, 3 },
  { "NewYears",     { {255,215,0}, {255,69,0}, {138,43,226} }, 3 },
  { "StPatrick",    { {0,128,0}, {255,255,255}, {255,165,0} }, 3 },
  { "Summer",       { {255,140,0}, {255,215,0}, {135,206,250} }, 3 },
  { "Winter",       { {0,191,255}, {255,250,250}, {176,196,222} }, 3 },
  { "Autumn",       { {210,105,30}, {244,164,96}, {205,133,63} }, 3 },
  { "Spring",       { {60,179,113}, {144,238,144}, {152,251,152} }, 3 },
  { "Rainbow",      { {255,0,0}, {255,127,0}, {255,255,0}, {0,255,0}, 
                      {0,0,255}, {75,0,130}, {148,0,211} }, 7 },
  { "Ocean",        { {0,105,148}, {72,209,204}, {0,191,255} }, 3 },
  { "Sunset",       { {255,69,0}, {255,140,0}, {255,215,0} }, 3 },
  { "Aurora",       { {72,61,139}, {123,104,238}, {65,105,225} }, 3 },
  { "Fireworks",    { {255,0,0}, {0,255,0}, {0,0,255}, {255,255,0} }, 4 },
  { "Galaxy",       { {75,0,130}, {138,43,226}, {0,0,0} }, 3 },
  { "Neon",         { {57,255,20}, {255,20,147}, {0,255,255} }, 3 },
  { "Pastel",       { {255,182,193}, {255,228,225}, {176,224,230} }, 3 },
  { "Vintage",      { {128,0,0}, {128,128,0}, {0,128,128} }, 3 },
  { "Tropical",     { {255,105,180}, {0,206,209}, {255,165,0} }, 3 },
  { "Forest",       { {34,139,34}, {107,142,35}, {85,107,47} }, 3 },
  { "Desert",       { {210,180,140}, {244,164,96}, {218,165,32} }, 3 },
  { "Cyberpunk",    { {255,0,255}, {0,255,255}, {255,255,0} }, 3 },
  { "Lava",         { {255,69,0}, {255,140,0}, {139,0,0} }, 3 },
  { "Ice",          { {173,216,230}, {176,224,230}, {135,206,250} }, 3 }
};
const int numColorSchemes = sizeof(colorSchemes) / sizeof(ColorScheme);

// LED animation timing
unsigned long ledAnimStartTime = 0;

// ----------------- Web Server on port 80 -------------
WebServer server(80);

// Scrolling text
int scrollMsgWidth;
int scrollX;
unsigned long lastFrameTime = 0;

// Marquee geometry
const int effectiveTop    = 127;
const int effectiveRight  = 63;
const int effectiveBottom = 127;
const int effectiveLeft   = 63;
const int effectivePerimeter = effectiveTop + effectiveRight + effectiveBottom + effectiveLeft;
const int blockCycle = 10;
float offsetEven = 0.0f;
float offsetOdd  = 3.0f;

// ===========================
//        ESP-Now Callback
// ===========================
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(sensorData)) {
    sensorData receivedData;
    memcpy(&receivedData, incomingData, sizeof(sensorData));
    latestData.temperature = receivedData.temperature;
    latestData.humidity    = receivedData.humidity;
    Serial.print("Received: Temp=");
    Serial.print(receivedData.temperature);
    Serial.print("C, Hum=");
    Serial.println(receivedData.humidity);
  } else {
    Serial.println("Received data with unexpected length");
  }
}

// ===========================
//     NVS Config Routines
// ===========================
void loadConfig() {
  preferences.begin("config", false);
  marqueeEnabled        = preferences.getBool("marqueeEnabled",        marqueeEnabled);
  scrollingEnabled      = preferences.getBool("scrollingEnabled",      scrollingEnabled);
  String sText          = preferences.getString("scrollingText",       scrollingText);
  updateInterval        = preferences.getULong("updateInterval",       updateInterval);
  String tPos           = preferences.getString("topPosition",         topPosition);
  String mPos           = preferences.getString("middlePosition",      middlePosition);
  ledEnabled            = preferences.getBool("ledEnabled",            ledEnabled);
  humidityThresholdLow  = preferences.getFloat("humidityThresholdLow", humidityThresholdLow);
  humidityThresholdHigh = preferences.getFloat("humidityThresholdHigh",humidityThresholdHigh);
  ledIntensity          = preferences.getInt("ledIntensity",           ledIntensity);
  ledAnimSpeed          = preferences.getULong("ledAnimSpeed",         ledAnimSpeed);
  String anim           = preferences.getString("ledAnimationMode",    ledAnimationMode);
  String scheme         = preferences.getString("ledColorScheme",      ledColorScheme);
  preferences.end();

  // Copy data back into global variables
  strncpy(scrollingText, sText.c_str(), sizeof(scrollingText));
  strncpy(topPosition,   tPos.c_str(),  sizeof(topPosition));
  strncpy(middlePosition,mPos.c_str(), sizeof(middlePosition));
  strncpy(ledAnimationMode,anim.c_str(),   sizeof(ledAnimationMode));
  strncpy(ledColorScheme, scheme.c_str(),  sizeof(ledColorScheme));
}

void saveConfig() {
  preferences.begin("config", false);
  preferences.putBool("marqueeEnabled",        marqueeEnabled);
  preferences.putBool("scrollingEnabled",      scrollingEnabled);
  preferences.putString("scrollingText",       String(scrollingText));
  preferences.putULong("updateInterval",       updateInterval);
  preferences.putString("topPosition",         String(topPosition));
  preferences.putString("middlePosition",      String(middlePosition));
  preferences.putBool("ledEnabled",            ledEnabled);
  preferences.putFloat("humidityThresholdLow", humidityThresholdLow);
  preferences.putFloat("humidityThresholdHigh",humidityThresholdHigh);
  preferences.putInt("ledIntensity",           ledIntensity);
  preferences.putULong("ledAnimSpeed",         ledAnimSpeed);
  preferences.putString("ledAnimationMode",    String(ledAnimationMode));
  preferences.putString("ledColorScheme",      String(ledColorScheme));
  preferences.end();
}

// ===========================
//      Web Server Handlers
// ===========================
void handleRoot() {
  // Serves the main HTML from webpage.h
  server.send_P(200, "text/html", MAIN_page);
}

void handleGetSensorData() {
  // Return current sensor readings as JSON
  StaticJsonDocument<200> doc;
  doc["temperature"] = latestData.temperature;
  doc["humidity"]    = latestData.humidity;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetConfig() {
  // Return current config as JSON
  StaticJsonDocument<400> doc;
  doc["marqueeEnabled"]        = marqueeEnabled;
  doc["scrollingEnabled"]      = scrollingEnabled;
  doc["scrollingText"]         = scrollingText;
  doc["updateInterval"]        = updateInterval;
  doc["topPosition"]           = topPosition;
  doc["middlePosition"]        = middlePosition;
  doc["ledEnabled"]            = ledEnabled;
  doc["humidityThresholdLow"]  = humidityThresholdLow;
  doc["humidityThresholdHigh"] = humidityThresholdHigh;
  doc["ledIntensity"]          = ledIntensity;
  doc["ledAnimSpeed"]          = ledAnimSpeed;
  doc["ledAnimationMode"]      = ledAnimationMode;
  doc["ledColorScheme"]        = ledColorScheme;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleUpdateConfig() {
  // Update config with JSON from client
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request");
    return;
  }
  String body = server.arg("plain");
  
  StaticJsonDocument<400> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }
  
  // Convert some values from String to bool
  marqueeEnabled        = (String(doc["marqueeEnabled"].as<String>()) == "1");
  scrollingEnabled      = (String(doc["scrollingEnabled"].as<String>()) == "1");
  ledEnabled            = (String(doc["ledEnabled"].as<String>()) == "1");
  
  // Copy string values
  if (doc["topPosition"])    strncpy(topPosition,    doc["topPosition"],    sizeof(topPosition) - 1);
  if (doc["middlePosition"]) strncpy(middlePosition, doc["middlePosition"], sizeof(middlePosition) - 1);
  if (doc["scrollingText"])  strncpy(scrollingText,  doc["scrollingText"],  sizeof(scrollingText) - 1);
  
  updateInterval         = doc["updateInterval"].as<unsigned long>();
  humidityThresholdLow   = doc["humidityThresholdLow"].as<float>();
  humidityThresholdHigh  = doc["humidityThresholdHigh"].as<float>();
  ledIntensity           = doc["ledIntensity"].as<int>();
  ledAnimSpeed           = doc["ledAnimSpeed"].as<unsigned long>();
  
  if (doc["ledAnimationMode"]) strncpy(ledAnimationMode, doc["ledAnimationMode"], sizeof(ledAnimationMode)-1);
  if (doc["ledColorScheme"])   strncpy(ledColorScheme,   doc["ledColorScheme"],   sizeof(ledColorScheme)-1);

  // Re-calc width if scrolling text changed
  u8g2.setFont(u8g2_font_chargen_92_mf);
  scrollMsgWidth = u8g2.getStrWidth(scrollingText);
  scrollX = 128;
  
  // Persist
  saveConfig();
  
  server.send(200, "text/plain", "Configuration Updated");
}

// ===========================
//       Webpage Setup
// ===========================
void setupWebpage() {
  WiFi.softAP("Temp_Monitor", ""); // Create a WiFi AP
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Define URL endpoints
  server.on("/", handleRoot);
  server.on("/getSensorData", handleGetSensorData);
  server.on("/getConfig",     handleGetConfig);
  server.on("/updateConfig",  HTTP_POST, handleUpdateConfig);
  
  server.begin();
  Serial.println("Web server started");
}

// ===========================
//   Marquee Dot Helper
// ===========================
void getBlockCoordinates(int pos, int &x, int &y) {
  // Distinguish perimeter edges
  if (pos < effectiveTop) {
    x = pos;       y = 0;
  } else if (pos < (effectiveTop + effectiveRight)) {
    x = 126;       y = pos - effectiveTop;
  } else if (pos < (effectiveTop + effectiveRight + effectiveBottom)) {
    int offset = pos - (effectiveTop + effectiveRight);
    x = 126 - offset;  y = 62;
  } else {
    int offset = pos - (effectiveTop + effectiveRight + effectiveBottom);
    x = 0;        y = 62 - offset;
  }
}

// ===========================
//   Display Render Helpers
// ===========================
void renderTemperature(const char* mode, int x, int y, bool useWhole = false) {
  // Prepare strings for Celsius & Fahrenheit
  float tempC = latestData.temperature;
  float tempF = (tempC * 9.0f / 5.0f) + 32.0f;
  char cNumStr[16], fNumStr[16];
  
  if (strcmp(mode, "tempBoth") == 0) {
    snprintf(cNumStr, sizeof(cNumStr), "%.2f", tempC);
    snprintf(fNumStr, sizeof(fNumStr), "%.2f", tempF);
  } 
  else if (strcmp(mode, "tempC") == 0) {
    if (useWhole) snprintf(cNumStr, sizeof(cNumStr), "%d", (int)tempC);
    else          snprintf(cNumStr, sizeof(cNumStr), "%.2f", tempC);
  } 
  else if (strcmp(mode, "tempF") == 0) {
    if (useWhole) snprintf(fNumStr, sizeof(fNumStr), "%d", (int)tempF);
    else          snprintf(fNumStr, sizeof(fNumStr), "%.2f", tempF);
  }
  
  // If x < 0, center it horizontally
  int tempX = x;
  if (tempX < 0) {
    int totalWidth = 0;
    if (strcmp(mode, "tempBoth") == 0) {
      totalWidth = u8g2.getStrWidth(cNumStr) + 
                   u8g2.getStrWidth("\xB0") + u8g2.getStrWidth("C") +
                   u8g2.getStrWidth(" / ") +
                   u8g2.getStrWidth(fNumStr) +
                   u8g2.getStrWidth("\xB0") + u8g2.getStrWidth("F");
    } 
    else if (strcmp(mode, "tempC") == 0) {
      totalWidth = u8g2.getStrWidth(cNumStr) + 
                   u8g2.getStrWidth("\xB0") + u8g2.getStrWidth("C");
    }
    else if (strcmp(mode, "tempF") == 0) {
      totalWidth = u8g2.getStrWidth(fNumStr) + 
                   u8g2.getStrWidth("\xB0") + u8g2.getStrWidth("F");
    }
    tempX = (128 - totalWidth) / 2;
  }
  
  // Draw selected mode
  if (strcmp(mode, "tempBoth") == 0) {
    u8g2.drawStr(tempX, y, cNumStr);
    tempX += u8g2.getStrWidth(cNumStr);
    u8g2.drawGlyph(tempX, y, 0x00B0);  // ° symbol
    tempX += u8g2.getStrWidth("\xB0");
    u8g2.drawStr(tempX, y, "C / ");
    tempX += u8g2.getStrWidth("C / ");
    u8g2.drawStr(tempX, y, fNumStr);
    tempX += u8g2.getStrWidth(fNumStr);
    u8g2.drawGlyph(tempX, y, 0x00B0);
    tempX += u8g2.getStrWidth("\xB0");
    u8g2.drawStr(tempX, y, "F");
  } 
  else if (strcmp(mode, "tempC") == 0) {
    u8g2.drawStr(tempX, y, cNumStr);
    tempX += u8g2.getStrWidth(cNumStr);
    u8g2.drawGlyph(tempX, y, 0x00B0);
    tempX += u8g2.getStrWidth("\xB0");
    u8g2.drawStr(tempX, y, "C");
  } 
  else if (strcmp(mode, "tempF") == 0) {
    u8g2.drawStr(tempX, y, fNumStr);
    tempX += u8g2.getStrWidth(fNumStr);
    u8g2.drawGlyph(tempX, y, 0x00B0);
    tempX += u8g2.getStrWidth("\xB0");
    u8g2.drawStr(tempX, y, "F");
  }
}

void renderHumidity(int x, int y) {
  char humStr[16];
  snprintf(humStr, sizeof(humStr), "%.1f%%", latestData.humidity);
  
  // Center horizontally if x < 0
  if (x < 0) {
    int width = u8g2.getStrWidth(humStr);
    x = (128 - width) / 2;
  }
  u8g2.drawStr(x, y, humStr);
}

// ===========================
//   LED Update Logic
// ===========================
void updateLEDAnimation() {
  led.setBrightness(ledIntensity);
  
  // Find chosen color scheme
  int schemeIndex = 0;
  for (int i = 0; i < numColorSchemes; i++) {
    if (strcasecmp(colorSchemes[i].name, ledColorScheme) == 0) {
      schemeIndex = i;
      break;
    }
  }
  ColorScheme cs = colorSchemes[schemeIndex];
  
  unsigned long now     = millis();
  unsigned long elapsed = now - ledAnimStartTime;
  unsigned long cycleTime = ledAnimSpeed;
  
  // Fade: smoothly transitions between color sets
  if (strcmp(ledAnimationMode, "fade") == 0) {
    int colorIndex1 = (elapsed / cycleTime) % cs.count;
    int colorIndex2 = (colorIndex1 + 1) % cs.count;
    float t         = (elapsed % cycleTime) / float(cycleTime);
    
    uint8_t r = cs.colors[colorIndex1].r + 
                t * (cs.colors[colorIndex2].r - cs.colors[colorIndex1].r);
    uint8_t g = cs.colors[colorIndex1].g + 
                t * (cs.colors[colorIndex2].g - cs.colors[colorIndex1].g);
    uint8_t b = cs.colors[colorIndex1].b + 
                t * (cs.colors[colorIndex2].b - cs.colors[colorIndex1].b);
    
    led.setPixelColor(0, r, g, b);
  }
  // Bounce: goes from one color to next, then back
  else if (strcmp(ledAnimationMode, "bounce") == 0) {
    int phase       = (elapsed / cycleTime) % 2;
    int colorIndex1 = (elapsed / cycleTime) % cs.count;
    int colorIndex2 = (colorIndex1 + 1) % cs.count;
    float t         = (elapsed % cycleTime) / float(cycleTime);
    if (phase == 1) t = 1 - t;
    
    uint8_t r = cs.colors[colorIndex1].r + 
                t * (cs.colors[colorIndex2].r - cs.colors[colorIndex1].r);
    uint8_t g = cs.colors[colorIndex1].g + 
                t * (cs.colors[colorIndex2].g - cs.colors[colorIndex1].g);
    uint8_t b = cs.colors[colorIndex1].b + 
                t * (cs.colors[colorIndex2].b - cs.colors[colorIndex1].b);
    
    led.setPixelColor(0, r, g, b);
  }
  // Wiggle: random color transitions in short intervals
  else if (strcmp(ledAnimationMode, "wiggle") == 0) {
    static int lastColorIndex = 0;
    static int nextColorIndex = 0;
    static unsigned long lastChange = 0;
    
    // Every ~500 ms pick a new color
    if (now - lastChange > 500) {
      lastColorIndex = nextColorIndex;
      nextColorIndex = random(cs.count);
      lastChange = now;
    }
    float t = (now - lastChange) / 500.0;
    if (t > 1) t = 1;
    
    uint8_t r = cs.colors[lastColorIndex].r + 
                t * (cs.colors[nextColorIndex].r - cs.colors[lastColorIndex].r);
    uint8_t g = cs.colors[lastColorIndex].g + 
                t * (cs.colors[nextColorIndex].g - cs.colors[lastColorIndex].g);
    uint8_t b = cs.colors[lastColorIndex].b + 
                t * (cs.colors[nextColorIndex].b - cs.colors[lastColorIndex].b);
    
    led.setPixelColor(0, r, g, b);
  }
  // Static fallback
  else {
    led.setPixelColor(0, cs.colors[0].r, cs.colors[0].g, cs.colors[0].b);
  }
  led.show();
}

void updateRGBLED() {
  led.setBrightness(ledIntensity);
  
  // If LED is disabled, turn it off
  if (!ledEnabled) {
    led.setPixelColor(0, 0, 0, 0);
    led.show();
    return;
  }
  
  // "static" means show based on humidity thresholds (blue/green/red)
  if (strcmp(ledAnimationMode, "static") == 0) {
    float hum = latestData.humidity;
    if (hum < 0)   hum = 0;
    if (hum > 100) hum = 100;
    
    if      (hum < humidityThresholdLow)   led.setPixelColor(0, 0,   0,   255); // Blue
    else if (hum < humidityThresholdHigh)  led.setPixelColor(0, 0,   255, 0  ); // Green
    else                                   led.setPixelColor(0, 255, 0,   0  ); // Red
    
    led.show();
  } 
  else {
    // For fade/bounce/wiggle, run animation
    updateLEDAnimation();
  }
}

// ===========================
//  Main Display Rendering
// ===========================
void updateDisplay() {
  // Clear screen
  u8g2.clearBuffer();

  // Top line (e.g. small font)
  u8g2.setFont(u8g2_font_7x14_tf);
  if (strcmp(topPosition, "humidity") == 0) {
    renderHumidity(-1, 14);
  } else {
    renderTemperature(topPosition, -1, 14, false);
  }

  // Middle line (e.g. large font)
  u8g2.setFont(u8g2_font_osb29_tr);
  if ((strcmp(middlePosition, "tempC") == 0) || (strcmp(middlePosition, "tempF") == 0)) {
    renderTemperature(middlePosition, -1, 44, true);
  } else if (strcmp(middlePosition, "humidity") == 0) {
    renderHumidity(-1, 44);
  } else {
    renderTemperature(middlePosition, -1, 44, false);
  }

  // Scrolling text
  if (scrollingEnabled && scrollingText[0] != '\0') {
    u8g2.setFont(u8g2_font_chargen_92_mf);
    int scrollY = 61;
    u8g2.drawStr(scrollX, scrollY, scrollingText);
    scrollX--;
    // If text has moved fully offscreen, reset
    if (scrollX < -scrollMsgWidth) {
      scrollX = 128;
    }
  } else {
    scrollX = 128;
  }

  // Marquee dots
  if (marqueeEnabled) {
    int numBlocks = effectivePerimeter / blockCycle;
    for (int i = 0; i < numBlocks; i++) {
      int pos;
      // Even blocks scroll one way, odd blocks the other
      if (i % 2 == 0) {
        pos = (int)(offsetEven + i * blockCycle) % effectivePerimeter;
      } else {
        pos = (int)(offsetOdd + i * blockCycle) % effectivePerimeter;
      }
      int bx, by;
      getBlockCoordinates(pos, bx, by);
      u8g2.drawBox(bx, by, 1, 1);
    }
    offsetEven += marqueeLeftSpeed;
    if (offsetEven >= effectivePerimeter) offsetEven -= effectivePerimeter;
    
    offsetOdd -= marqueeRightSpeed;
    if (offsetOdd < 0) offsetOdd += effectivePerimeter;
  }

  // Render to display
  u8g2.sendBuffer();
}

// ===========================
//        Setup / Loop
// ===========================
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { /* wait if needed */ }

  Serial.println("Starting...");

  // I2C pins for the OLED (change if needed)
  Wire.begin(12, 13);

  u8g2.begin();
  u8g2.disableUTF8Print();
  
  // Prep scrolling text size
  u8g2.setFont(u8g2_font_chargen_92_mf);
  scrollMsgWidth = u8g2.getStrWidth(scrollingText);
  scrollX = 128;
  
  // LED
  led.begin();
  led.setBrightness(ledIntensity);
  led.show();
  ledAnimStartTime = millis();
  
  // Load stored settings
  loadConfig();

  // Setup ESP-Now
  WiFi.mode(WIFI_AP_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-Now");
  } else {
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("ESP-Now ready to receive");
  }
  
  // Start the web server
  setupWebpage();
  
  Serial.println("Setup complete!");
}

void loop() {
  server.handleClient(); // handle any web requests
  
  unsigned long now = millis();
  if (now - lastFrameTime >= updateInterval) {
    lastFrameTime = now;
    updateRGBLED();  // Update LED based on config
    updateDisplay(); // Refresh OLED
  }
  
  delay(1);
}
