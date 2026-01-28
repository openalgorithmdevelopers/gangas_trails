#include <RadioLib.h>
#include <TinyGPSPlus.h>

// TinyGPS++ object
TinyGPSPlus gps;

// Use UART2 on ESP32
HardwareSerial GPSserial(2);

// GPS baud rate
static const uint32_t GPSBaud = 9600;
// Pins: CS: 5, DIO1: 2, NRST: 14, BUSY: 32
SX1262 radio = new Module(5, 2, 14, 32);

void setup() {
  Serial.begin(115200);

  	Serial.println("trying to connect to GPS");

  // RX = GPIO 16, TX = GPIO 17
  GPSserial.begin(GPSBaud, SERIAL_8N1, 16, 17);

  Serial.println(F("ESP32 + TinyGPS++ + NEO-6M"));
  Serial.print(F("TinyGPS++ Library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();


  Serial.print(F("[SX1262] Initializing ... "));

  // Initialize with your working settings
  int state = radio.begin(433.5, 250.0, 10, 6, 0xAB, 10, 15, 2.4);
  
  // NOTE: If your module uses DIO2 as an RF switch (common in Ebyte modules)
  radio.setDio2AsRfSwitch();

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  double lat, lng;
  float speed;
  bool geoFlag;
  while (GPSserial.available() > 0) {
    if (gps.encode(GPSserial.read())) {
       getGeoData(&lat, &lng, &speed, &geoFlag);
//       displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

  Serial.print(F("[SX1262] Transmitting packet ... "));
  
  int state;
  state = transmitGeoData(lat, lng, speed, &geoFlag);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("finished!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  delay(2000); // Wait 2 seconds before next transmit
}

void getGeoData(double *lat, double *lng, float *speedKmph, bool *geoFlag) {
  if (gps.location.isValid()) {
    *lat = gps.location.lat();
    *lng = gps.location.lng();
    *speedKmph = gps.speed.kmph();  // km/h

    *geoFlag = true;
  } else {
    *geoFlag = false;
  }
}

int transmitGeoData(double lat, double lng, float speed, bool geoFlag) {
  int state;
  if(!geoFlag){
     state = radio.transmit("INVALID");
     return state;
  }
    
  int32_t lat_i   = (int32_t)(lat * 100000);   // ~1m accuracy
  int32_t lng_i   = (int32_t)(lng * 100000);
  uint16_t spd_i  = (uint16_t)(speed * 10);    // 0.1 km/h

  String payload = "";
  payload += String(lat_i);
  payload += ",";
  payload += String(lng_i);
  payload += ",";
  payload += String(spd_i);

  Serial.print("TX: ");
  Serial.println(payload);

  state = radio.transmit(payload);
  return state;
}
