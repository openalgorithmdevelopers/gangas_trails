#include <WiFi.h>
#include "esp_wpa2.h"
#include <HTTPClient.h>
#include <TinyGPSPlus.h>

#define EAP_IDENTITY "upesddn\bhupendra.singh"
#define EAP_USERNAME "bhupendra.singh"
#define EAP_PASSWORD "Oct2018@"
#define WIFI_SSID "UPESNET"

const char* deviceName = "Tracker_Ganga_01"; // Change this for different devices

#define LED_PIN 2
// const char* googleScriptURL =
//   "https://script.google.com/macros/s/AKfycbxxxx/exec";
const char* googleScriptURL =
"https://script.google.com/macros/s/AKfycbzXsHn_Ly6bbhiYfnq4zpNMgmLBCBPbihQDrWj7HFnL6CFAyqIxVn4brjwZsxgaxith4g/exec";

int sendCount = 0;
unsigned long lastSend = 0;

TinyGPSPlus gps;

// Use UART2 on ESP32
HardwareSerial GPSserial(2);

// GPS baud rate
static const uint32_t GPSBaud = 9600;

struct GPSData {
  bool valid;
  double lat;
  double lng;
};


void setup() {
  Serial.begin(115200);
  Serial.println("inside setup");

  connect_wifi();

  Serial.println("trying to connect to GPS");

  // RX = GPIO 16, TX = GPIO 17
  GPSserial.begin(GPSBaud, SERIAL_8N1, 16, 17);

  Serial.println(F("ESP32 + TinyGPS++ + NEO-6M"));
  Serial.print(F("TinyGPS++ Library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  pinMode(LED_PIN, OUTPUT);
}

void connect_wifi()
{
    WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  Serial.println("trying to connect");
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to University Wi-Fi!");
  Serial.println(WiFi.localIP());
}


void loop() {
  GPSData gpsData;
  while (GPSserial.available() > 0) {
    if (gps.encode(GPSserial.read())) {
      gpsData = getLatLong();
      if(!gpsData.valid)
      {
        analogWrite(LED_PIN, 10);
        Serial.println("error getting gps data");
      }
      else{ 
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Lat, long : ");
        Serial.println(gpsData.lat);

              
        if (sendCount >= 100) {
          Serial.println("Finished sending 10 entries.");
          while (true);   // stop
        }

        if (millis() - lastSend >= 5000) {
          lastSend = millis();

          // ---- Dummy GPS data ----
          // float latitude  = 30.3165 + sendCount * 0.0001;
          // float longitude = 78.0322 + sendCount * 0.0001;

          // sendToGoogleSheet(latitude, longitude);
          sendToGoogleSheet(deviceName, gpsData.lat, gpsData.lng);
          sendCount++;
        }
  
      }
      delay(3000);
      digitalWrite(LED_PIN, LOW);
      delay(2000);

    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}


GPSData getLatLong() {
  GPSData data;

  if (gps.location.isValid()) {
    data.valid = true;
    data.lat = gps.location.lat();
    data.lng = gps.location.lng();
  } else {
    data.valid = false;
    data.lat = 0.0;
    data.lng = 0.0;
  }

  return data;
}


void sendToGoogleSheet(const char* name, float lat, float lng) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  HTTPClient http;

  // Added &name= parameter to the URL string
  String url = String(googleScriptURL) +
               "?name=" + String(name) +
               "&lat=" + String(lat, 6) +
               "&lng=" + String(lng, 6);

  http.begin(url);
  Serial.println("printing url");
  Serial.println(url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // Essential for Google Scripts
  
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.print("Sent data for ");
    Serial.print(name);
    Serial.print(" → HTTP ");
    Serial.println(httpCode);
  } else {
    Serial.print("Failed to send data, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}