#include <RadioLib.h>

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
}

void loop() {
  Serial.print(F("[SX1262] Waiting for incoming transmission ... "));

  String str;
  int state = radio.receive(str);

  if (state == RADIOLIB_ERR_NONE) {
    // Packet was successfully received
    Serial.println(F("success!"));
    Serial.print(F("[SX1262] Data:\t\t"));
    Serial.println(str);

    // Print RSSI (Signal Strength)
    Serial.print(F("[SX1262] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    // Timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));
  } else {
    // Some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}