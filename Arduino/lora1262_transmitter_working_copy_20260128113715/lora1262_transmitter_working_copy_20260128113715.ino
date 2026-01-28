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

double lat, lng;
bool geoFlag = false;
void loop() {

  while (GPSserial.available() > 0) {
    if (gps.encode(GPSserial.read())) {
      getLatLong(&lat, &lng);
      // displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

  Serial.print(F("[SX1262] Transmitting packet ... "));

  // Send the message
  String geoData = "";
  geoData = "lat:" + String(int(lat) + ", long:" + String(int(lng)));
  Serial.println(geoData);
  int state;
  if(geoFlag)
    state = radio.transmit(geoData);
  else
    state = radio.transmit("Hello World!");
  // state = radio.transmit("Hello World!");
  // state = radio.transmit(geoData);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("finished!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  delay(2000); // Wait 2 seconds before next transmit
}

void getLatLong(double *lat, double *lng){
  // Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    *lat = gps.location.lat();
    *lng = gps.location.lng();
    geoFlag = true;
  } else {
    Serial.print(F("INVALID"));
    geoFlag = false;
  }
}
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(", "));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  // Serial.print(F("  Date/Time: "));
  // if (gps.date.isValid()) {
  //   Serial.print(gps.date.day());
  //   Serial.print(F("/"));
  //   Serial.print(gps.date.month());
  //   Serial.print(F("/"));
  //   Serial.print(gps.date.year());
  // } else {
  //   Serial.print(F("INVALID"));
  // }

  // Serial.print(F(" "));
  // if (gps.time.isValid()) {
  //   if (gps.time.hour() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.hour());
  //   Serial.print(F(":"));
  //   if (gps.time.minute() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.minute());
  //   Serial.print(F(":"));
  //   if (gps.time.second() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.second());
  // } else {
  //   Serial.print(F("INVALID"));
  // }

  Serial.println();
}