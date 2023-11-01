// NAME: PN5180-ReadUID.ino
//
// DESC: Example usage of the PN5180 library for the PN5180-NFC Module
//       from NXP Semiconductors.
//
// Copyright (c) 2018 by Andreas Trappmann. All rights reserved.
// Copyright (c) 2019 by Dirk Carstensen.
//
// This file is part of the PN5180 library for the Arduino environment.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// BEWARE: SPI with an Arduino to a PN5180 module has to be at a level of 3.3V
// use of logic-level converters from 5V->3.3V is absolutly neccessary
// on most Arduinos for all input pins of PN5180!
// If used with an ESP-32, there is no need for a logic-level converter, since
// it operates on 3.3V already.
//
// ESP-32    <--> PN5180 pin mapping:
// 3.3V      <--> 3.3V
// GND       <--> GND
// SCLK, 18   --> SCLK
// MISO, 19  <--  MISO
// MOSI, 23   --> MOSI
// SS, 16     --> NSS (=Not SS -> active LOW)
// BUSY, 5   <--  BUSY
// Reset, 17  --> RST
//

/*
 * Pins on ICODE2 Reader Writer:
 *
 *   ICODE2   |     PN5180
 * pin  label | pin  I/O  name
 * 1    +5V
 * 2    +3,3V
 * 3    RST     10   I    RESET_N (low active)
 * 4    NSS     1    I    SPI NSS
 * 5    MOSI    3    I    SPI MOSI
 * 6    MISO    5    O    SPI MISO
 * 7    SCK     7    I    SPI Clock
 * 8    BUSY    8    O    Busy Signal
 * 9    GND     9  Supply VSS - Ground
 * 10   GPIO    38   O    GPO1 - Control for external DC/DC
 * 11   IRQ     39   O    IRQ
 * 12   AUX     40   O    AUX1 - Analog/Digital test signal
 * 13   REQ     2?  I/O   AUX2 - Analog test bus or download
 *
 */
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

// #include <PN5180.h>
// #include <PN5180ISO14443.h>

// #define PN5180_NSS_1  16   
// #define PN5180_BUSY_1 5  
// #define PN5180_RST_1  17

//#define PN5180_NSS_2  16   
//#define PN5180_BUSY_2 5  
//#define PN5180_RST_2  17

WiFiMulti wifiMulti;

// PN5180ISO14443 nfc14443_1(PN5180_NSS_1, PN5180_BUSY_1, PN5180_RST_1);
//PN5180ISO14443 nfc14443_2(PN5180_NSS_2, PN5180_BUSY_2, PN5180_RST_2);

String laburlkey = "https://zhuan-ti-hou-duan.onrender.com/pushIdTest";

String wifiid = "";
String wifipw = "";


void setup() {
  Serial.begin(115200);
  // Serial.println(F("=================================="));
  // Serial.println(F("Uploaded: " __DATE__ " " __TIME__));
  // Serial.println(F("PN5180 ISO14443 Demo Sketch"));

  // nfc14443_1.begin();
  // nfc14443_2.begin();
  // for(uint8_t t = 4; t > 0; t--) {
  //       Serial.printf("[SETUP] WAIT %d...\n", t);
  //       Serial.flush();
  //       delay(1000);
  //   }

  wifiMulti.addAP("310_Lab", "TAHRD310");
  wifiMulti.addAP("wifi_TAHRD-503", "tahrd503");
  wifiMulti.addAP(wifiid, wifipw);
}

// uint32_t loopCnt = 0;


// read cards loop
void loop() {
  // Serial.println(F("----------------------------------"));
  // Serial.print(F("Loop #"));
  // Serial.println(loopCnt++);
  #if defined(ARDUINO_ARCH_ESP32)  
    Serial.println("Free heap: " + String(ESP.getFreeHeap())); 
  #endif
  // uint8_t uid[10];
  // String full_uid;
  // check for ISO-14443 card
  nfc14443_1.reset();
  Serial.print(F("ISO14443 card found, UID="));
  nfc14443_1.setupRF();
  if (nfc14443_1.isCardPresent()) {
    uint8_t uidLength = nfc14443_1.readCardSerial(uid);
    if (uidLength > 0) {
      Serial.print(F("ISO14443 card found, UID="));
      for (int i=0; i<uidLength; i++) {
        Serial.print(uid[i] < 0x10 ? " 0" : " ");
        Serial.print(uid[i], HEX);
        full_uid += String(uid[i], HEX);
        }
      Serial.println();
      Serial.println(F("----------------------------------"));
      if(wifiMulti.run() == WL_CONNECTED){
        HTTPClient http;
        http.begin(laburlkey+"?id="+full_uid);
        Serial.print(laburlkey+"?id="+full_uid);
        int httpCode = http.GET();
        http.end();
      }
      else{
        Serial.println("wifi not connected");
      }
      delay(1000); 
      return;
    }
  } 
  
  // no card detected
  Serial.println(F("*** No card detected!"));
}
