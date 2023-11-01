#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

WiFiMulti wifiMulti;

String laburlkey = "https://zhuan-ti-hou-duan.onrender.com/getLastOnCar";
String data;

const char * wifiid = "a";//place wifi    id     here
const char * wifipw = "a";//place wifi password  here

void setup() {
  Serial.begin(115200);

  wifiMulti.addAP("310_Lab", "TAHRD310");
  wifiMulti.addAP("wifi_TAHRD-503", "tahrd503");
  wifiMulti.addAP(wifiid, wifipw );
}

// read cards loop
void loop() {
  #if defined(ARDUINO_ARCH_ESP32)  
    Serial.println("Free heap: " + String(ESP.getFreeHeap())); 
  #endif

  if(wifiMulti.run() == WL_CONNECTED){
    HTTPClient http;
    http.begin(laburlkey);
    http.end();
  }
  else{
    Serial.println("wifi not connected");
  }
  // no card detected
  Serial.println(F("*** No card detected!"));
}
