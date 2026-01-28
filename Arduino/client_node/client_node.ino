#include <RadioLib.h>
#include <WiFi.h>
#include "esp_wpa2.h"

#define EAP_IDENTITY "upesddn\bhupendra.singh"
#define EAP_USERNAME "bhupendra.singh"
#define EAP_PASSWORD "Oct2018@"
#define WIFI_SSID "UPESNET"

SX1262 radio = new Module(5, 2, 14, 32);

void setup() {
  Serial.begin(115200);
  Serial.print(F("[SX1262] Initializing p... "));

  // Settings must MATCH the transmitter exactly
  int state = radio.begin(433.5, 250.0, 10, 6, 0xAB, 10, 15, 2.4);
  radio.setDio2AsRfSwitch();

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  connect_wifi();

}

void loop() {
  Serial.print(F("[SX1262] Waiting for incoming transmission ... "));
String str;
int state = radio.receive(str);

if (state == RADIOLIB_ERR_NONE) {

  Serial.println(F("success!"));
  Serial.print(F("[SX1262] Raw Data:\t"));
  Serial.println(str);

  // Print RSSI
  Serial.print(F("[SX1262] RSSI:\t\t"));
  Serial.print(radio.getRSSI());
  Serial.println(F(" dBm"));

  // ---------- GEO DATA PARSING ----------
  int32_t lat_i, lng_i;
  uint16_t spd_i;

  if (sscanf(str.c_str(), "%ld,%ld,%hu", &lat_i, &lng_i, &spd_i) == 3) {

    double latitude  = lat_i / 1e5;
    double longitude = lng_i / 1e5;
    float speed_kmh  = spd_i / 10.0;

    Serial.println(F("Parsed Geo Data:"));
    Serial.print(F("Latitude : "));
    Serial.println(latitude, 5);

    Serial.print(F("Longitude: "));
    Serial.println(longitude, 5);

    Serial.print(F("Speed    : "));
    Serial.print(speed_kmh, 1);
    Serial.println(F(" km/h"));

    // 👉 This is where DB / MQTT code goes

  } else {
    Serial.println(F("Invalid GEO packet format"));
  }

  } 
  else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println(F("timeout!"));
  } 
  else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

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