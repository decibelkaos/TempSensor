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
    This file reads temperature and humidity data from an AHT10 sensor and 
    broadcasts it using ESP‑Now. The broadcast address is set to all 0xFF, 
    meaning any ESP-Now receiver can pick up the data.
*/

#include <Wire.h>
#include <AHTxx.h>
#include <WiFi.h>
#include <esp_now.h>

// Create an instance of the AHT10 sensor at address 0x38 (type AHT2x)
AHTxx aht10(AHTXX_ADDRESS_X38, AHT2x_SENSOR);

// Define a structure to hold both temperature and humidity
typedef struct sensorData {
  float temperature;
  float humidity;
} sensorData;

// Broadcast address for ESP-Now (all 0xFF = broadcast)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback for when data is sent via ESP-Now
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Non-blocking timer to control sensor reads / transmissions
unsigned long previousMillis = 0;
const unsigned long updateInterval = 20;  // read/send every 20 ms

void setup() {
  Serial.begin(115200);
  // Initialize I2C on GPIO12 (SDA) and GPIO13 (SCL) for the AHT10
  Wire.begin(12, 13);
  Wire.setClock(100000);

  // Initialize AHT10 sensor, retry until successful
  while (!aht10.begin()) {
    Serial.println(F("AHT10 not found or failed to load calibration data"));
    delay(5000);
  }
  Serial.println(F("AHT10 successfully initialized!"));

  // Set WiFi mode to Station (required for ESP-Now)
  WiFi.mode(WIFI_STA);

  // Initialize ESP-Now
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP‑Now");
    return;
  }

  // Register our "sent" callback
  esp_now_register_send_cb(OnDataSent);

  // Prepare broadcast peer info
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;      // use the default channel
  peerInfo.encrypt = false;  // no encryption

  // Add broadcast peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add broadcast peer");
    return;
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if time to read sensor & send data
  if (currentMillis - previousMillis >= updateInterval) {
    previousMillis = currentMillis;

    // Read temperature in Celsius
    float temperatureC = aht10.readTemperature();
    if (temperatureC == AHTXX_ERROR) {
      Serial.println(F("Error reading temperature"));
    } else {
      float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
      Serial.print(F("Temperature: "));
      Serial.print(temperatureC);
      Serial.print(F(" °C / "));
      Serial.print(temperatureF);
      Serial.println(F(" °F"));
    }

    // Read humidity (in %)
    float humidity = aht10.readHumidity();
    if (humidity == AHTXX_ERROR) {
      Serial.println(F("Error reading humidity"));
    } else {
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.println(F(" %"));
    }
    Serial.println(F("---------------------"));

    // Create a struct to hold the values
    sensorData data;
    data.temperature = temperatureC;
    data.humidity    = humidity;

    // Send struct data via ESP‑Now
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending data");
    }
  }
}
