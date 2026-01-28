#include <WiFi.h>
#include "esp_wpa2.h"

#define EAP_IDENTITY "upesddn\bhupendra.singh"
#define EAP_USERNAME "bhupendra.singh"
#define EAP_PASSWORD "Oct2018@"
#define WIFI_SSID "UPESNET"

void setup() {
  Serial.begin(115200);

  connect_wifi();
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

void loop() 
{
  
}
