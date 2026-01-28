#include <TinyGPSPlus.h>
#include <WiFi.h>

// TinyGPS++ object
  TinyGPSPlus gps;

  // Use UART2 on ESP32
  HardwareSerial GPSserial(2);

// GPS baud rate
static const uint32_t GPSBaud = 9600;

void setup() {
  Serial.begin(115200);

	Serial.println("trying to connect to GPS");

  // RX = GPIO 16, TX = GPIO 17
  GPSserial.begin(GPSBaud, SERIAL_8N1, 16, 17);

  Serial.println(F("ESP32 + TinyGPS++ + NEO-6M"));
  Serial.print(F("TinyGPS++ Library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
}

void loop() {
  while (GPSserial.available() > 0) {
    if (gps.encode(GPSserial.read())) {
      displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
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
