// #include <Arduino.h>

// #include <WiFi.h>
// #include <WiFiMulti.h>

// #include <HTTPClient.h>

// WiFiMulti wifiMulti;

// String laburlkey = "https://zhuan-ti-hou-duan.onrender.com/pushIdTest";

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  // Serial.println("Reader System Start\r\n");

  // wifiMulti.addAP("310_Lab", "TAHRD310");
  //wifiMulti.addAP("wifi_TAHRD-503", "tahrd503");
}

unsigned char CheckIsHexChar(unsigned char bChar){
  if(bChar >= '0' && bChar <= '9') return 1;
  else if(bChar >= 'A' && bChar <= 'F') return 1;
  else if(bChar >= 'a' && bChar <= 'f') return 1;
  else return 0;
}

char rxdata = 0;
char rxstate = 0;
unsigned char bReadLength = 0;
String strTagID = "";

// read cards loop
void loop() {
  while(Serial2.available()>0){
    rxdata = Serial2.read();
    Serial.println(rxdata); 
    switch(rxstate){
      case 0:  //get head
        if(rxdata == 0x02){
          rxstate = 1;
          strTagID = "";
          bReadLength = 0;
        }
        break;
      case 1:  //Get TagID
        if(rxdata != 0x03){ //Not End
          if(CheckIsHexChar(rxdata) == 1){ //check legal char
            strTagID = strTagID + rxdata;
            bReadLength++;
          }
          else{
            strTagID = "";
            rxstate = 0;
          }
        }
        else{
          if((bReadLength) % 2 == 0){ //TagLength(HEX data) is odd 
            if(strTagID != ""){
              Serial.print("TagID: ");
              Serial.println(strTagID);
              // if(wifiMulti.run() == WL_CONNECTED){
              //   HTTPClient http;
              //   http.begin(laburlkey+"?id="+strTagID);
              //   Serial.print(laburlkey+"?id="+strTagID);
              //   int httpCode = http.GET();
              //   http.end();
              // 
              // }
            }
            else{
              Serial.println(F("wifi not connected"));
            }
          }
          else{
            Serial.print(F("Unlegal Data\r\n"));
          }
          strTagID = "";
          rxstate = 0;
          bReadLength = 0;

        break;
    }
    delay(100);
  }
  // no card detected
}

