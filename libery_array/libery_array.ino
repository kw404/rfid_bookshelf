#include <Adafruit_NeoPixel.h>
#include "I2c_talker.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define SERIAL_BAUD 115200                                 // baud rate
#define REFRESH_RATE 50                                  // loop 刷新率
#define NUM_UNITS 10                                     // 書櫃單元數量
#define NUM_LEDS_PER_UNIT 1                              // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)   // 總LED數量
#define LED_PIN 11                                       // 連接第一個LED的腳位
int BUTTON_PINS[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 12 };  // 按鈕連接腳位

Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條

const char *ssid = "wifi_TAHRD-503";
const char *password = "tahrd503";

char now_state;

AsyncWebServer server(80);

struct UnitInfo {// 定義單元資訊的結構，包括書本資訊和是否佔用
  bool isOccupied;
  bool isArrived;
  char bookID[DATA_SIZE];
  char cabinetID[DATA_SIZE];
};
UnitInfo unitData[NUM_UNITS];  // 用來儲存每個單元的書本資訊和狀態

void setup() {
  Serial.begin(SERIAL_BAUD);
  leds.begin();
  leds.show();
  for (int unit = 0; unit < NUM_UNITS; unit++) {
    pinMode(BUTTON_PINS[unit], INPUT_PULLUP);
    unitData[unit].isOccupied = false;
    unitData[unit].isArrived = false;
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define web server routes
  server.on("/A", HTTP_GET, [](AsyncWebServerRequest *request){
    // Allow requests from your web server's domain
    A_shelf();
  });

  server.on("/B", HTTP_GET, [](AsyncWebServerRequest *request){
    // Allow requests from your web server's domain
    B_shelf();
  });

  // Start server
  server.begin();
  Wire.begin();                           // 初始化I2C通信
  Wire.beginTransmission(SLAVE_ADDRESS);  // 設定ESP32 S3的I2C地址
  Wire.endTransmission();

  powerOnLed();
}
void loop() {
  main_menu();
  char terminalMode;
  int i = 1;
  while (i) {
    if (Serial.available()) { terminalMode = Serial.read(); }
    switch (terminalMode) {
      case 'a':
      case 'A':
        deloper_mode();
        i = 0;
        terminalMode = 'n';
        break;
      case 'b':
      case 'B':
        robot_mode();
        i = 0;
        terminalMode = 'n';
        break;
      default:
        break;
    }
  }
}

void deloper_mode() {  //  開發者模式LOOP
  int i = 1;
  char instruction;
  deloper_mode_menu();
  while (i) {
    if (Serial.available()) { instruction = Serial.read(); }
    switch (instruction) {
      case 'a':
      case 'A':
        printUnitStatus();
        deloper_mode_menu();
        instruction = 'n';
        break;
      case 'b':
      case 'B':
        modify_unit_Arrived_status();
        arrived_blink();
        deloper_mode_menu();
        instruction = 'n';
        break;
      case 'e':
      case 'E':
        i = 0;
        instruction = 'n';
        break;
      default:
        break;
    }
    refreshStatus();
    delay(1000/REFRESH_RATE);  //刷新率
  }
}

void robot_mode() {  //  機器人模式LOOP
  int i = 1;
  char instruction;
  robot_mode_menu();
  while (i) {
    if (Serial.available()) { instruction = Serial.read(); }
    switch (instruction) {
      case 'a':
      case 'A':
        printUnitStatus();
        robot_mode_menu();
        instruction = 'n';
        break;
      case 'b':
      case 'B':
        modify_unit_Arrived_status();
        arrived_blink();
        robot_mode_menu();
        instruction = 'n';
        break;
      case 'e':
      case 'E':
        i = 0;
        instruction = 'n';
        break;
      default:
        break;
    }
    refreshStatus();
    delay(1000/REFRESH_RATE);  //刷新率
  }
}

void refreshStatus() {  //刷新所有單元的狀態並顯示於LED
  for (int unit = 0; unit < NUM_UNITS; unit++) {

    if (!digitalRead(BUTTON_PINS[unit]) && !unitData[unit].isOccupied) {  //按鈕被觸發且處於未占用狀態
      requestBookData(unitData[unit].bookID, unitData[unit].cabinetID);
      unitData[unit].isOccupied = !unitData[unit].isOccupied;                  // 設置為已佔用
    } else if (digitalRead(BUTTON_PINS[unit]) && unitData[unit].isOccupied) {  //按鈕沒被觸發且處於占用狀態
      for (int dataIndex = 0; dataIndex < DATA_SIZE; dataIndex++) {            //清空unitData[unit].bookID和unitData[unit].cabinetID
        unitData[unit].bookID[dataIndex] = '\0';
        unitData[unit].cabinetID[dataIndex] = '\0';
      }
      unitData[unit].isOccupied = !unitData[unit].isOccupied;  // 設置為未佔用
    }
    //led color
    setUnit_color(unit, unitData[unit].isOccupied, unitData[unit].isArrived);
  }
}

void setUnit_color(int unit, bool isOccupied, bool isArrived) {  //調整書櫃單元顏色
  if (isArrived) {
    for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
      setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 255, 71);  // 香蕉色
    }
  } else {
    if (isOccupied) {
      for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
        setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 0, 0);  // 紅色
      }
    } else {
      for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
        setColor(unit * NUM_LEDS_PER_UNIT + led, 0, 255, 0);  // 綠色
      }
    }
  }
}

void setColor(int ledNum, int red, int green, int blue) {  // 設定LED顏色
  leds.setPixelColor(ledNum, red, green, blue);
  leds.show();
}

void arrived_blink() {  //  在書本的抵達狀態被改動時, 閃四下燈
  Serial.println("書本已抵達書櫃, 開始閃爍屬於此書櫃的單元格");
  Serial.print("閃燈中");
  for (int i = 0; i < 4; i++) {
    for (int unit = 0; unit < NUM_UNITS; unit++) {  //  暗
      for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
        setColor(unit * NUM_LEDS_PER_UNIT + led, 0, 0, 0);
      }
    }
    delay(1000);
    for (int unit = 0; unit < NUM_UNITS; unit++) {  //  亮
      if (unitData[unit].isArrived) {
        for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
          setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 255, 71);  // 香蕉色
        }
      } else {
        for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
          setColor(unit * NUM_LEDS_PER_UNIT + led, 0, 0, 0);
        }
      }
    }
    delay(1000);

    Serial.print(" .");
  }
  Serial.println("閃爍完畢");
}

void powerOnLed() {  //  開機閃燈

  int frequency = 150;
  for (int unit = 0; unit < NUM_UNITS; unit++) {  //  一顆顆開綠色
    for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
      setColor(unit * NUM_LEDS_PER_UNIT + led, 0, 255, 0);
      delay(frequency);
    }
  }
  delay(100);
  for (int unit = 0; unit < NUM_UNITS; unit++) {  //  一顆顆開紅色
    for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
      setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 0, 0);
      delay(frequency);
    }
  }
  delay(100);
  for (int unit = 0; unit < NUM_UNITS; unit++) {  //  一顆顆開香蕉色
    for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
      setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 255, 71);
      delay(frequency);
    }
  }
  delay(1000);

  for (int i = 0; i < 2; i++) {
    for (int unit = 0; unit < NUM_UNITS; unit++) {  //  暗
      for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
        setColor(unit * NUM_LEDS_PER_UNIT + led, 0, 0, 0);
      }
    }
    delay(600);
    for (int unit = 0; unit < NUM_UNITS; unit++) {  //  亮
      for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
        setColor(unit * NUM_LEDS_PER_UNIT + led, 255, 255, 71);
      }
    }
    delay(600);
  }
}

void modify_unit_Arrived_status() {  //  調整書櫃抵達狀態的LOOP
  char instruction;
  printUnitStatus();
  Serial.println("開始調整到達狀態, 請輸入想要調整的單元編號, 若調整完, 輸入E鍵離開");
  while (1) {
    if (Serial.available()) { instruction = Serial.read(); }
    if (isnumber(instruction)) {
      modify(instruction);
      printUnitStatus();
    } else if (instruction == 'e' || instruction == 'E') {
      break;
    }
  }
}
void modify(int unit) {  //  調整書櫃抵達狀態
  unit = unit - 48;
  unitData[unit].isArrived = !unitData[unit].isArrived;
}
bool isnumber(char a) {  // 判斷serial讀取字元是否為數字0~9
  return (a >= '0' && a <= '9');
}

void A_shelf(){
  Serial.println("going to A shelf");
  for (int unit = 0; unit < NUM_UNITS; unit++){
    if(unitData[unit].cabinetID == "A"){
      setColor(unit * NUM_LEDS_PER_UNIT, 255, 255, 71);
    }
  }
  digitalWrite(LED_PIN,HIGH);
  delay(4000);
  digitalWrite(LED_PIN,LOW);
}

void B_shelf(){
  Serial.println("going to B shelf");
  for (int unit = 0; unit < NUM_UNITS; unit++){
    if(unitData[unit].cabinetID == "B"){
      setColor(unit * NUM_LEDS_PER_UNIT, 255, 255, 71);
    }
  }
  digitalWrite(LED_PIN,HIGH);
  delay(2000);
  digitalWrite(LED_PIN,LOW);
}

void printUnitStatus() {  // 列印書櫃狀態和書本資訊
  Serial.println("");
  Serial.println("書櫃狀態:");

  for (int unit = 0; unit < NUM_UNITS; unit++) {
    Serial.print("單元 ");
    Serial.print(unit);
    Serial.print(": ");
    if (unitData[unit].isOccupied) {
      Serial.print("close");
      Serial.print(" 書本ID: ");
      Serial.print(unitData[unit].bookID);
      Serial.print(" 所屬書櫃ID: ");
      Serial.print(unitData[unit].cabinetID);
    } else {
      Serial.print("open");
    }
    Serial.print(" 機器人是否到達該書櫃? ");
    if (unitData[unit].isArrived == true) {
      Serial.print("YES");
    } else {
      Serial.print("NO");
    }
    Serial.println("");
  }
}
void main_menu() {  // 列印主選單
  Serial.println(" ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println("//////////////////////////////// Libery Array System /////////////////////////////// ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println(" ");
  Serial.println("welcon to Libery Array System!");
  Serial.println("this is mani menu, please select working process mode:");
  Serial.println("a. deloper mode");
  Serial.println("b. robot mode");
  Serial.println("please choose mode:");
}
void deloper_mode_menu() {  // 列印開發者選單
  Serial.println(" ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println("/////////////////////////////////// deloper mode /////////////////////////////////// ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println(" ");
  Serial.println("this is unit information mode menu, please select working process mode:");
  Serial.println("a. print unit info");
  Serial.println("b. adjust unit Arrived status");
  Serial.println("e. back to main menu");
  Serial.println("please choose mode:");
}
void robot_mode_menu() {  // 列印機器人選單
  Serial.println(" ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println("//////////////////////////////////// robot mode //////////////////////////////////// ");
  Serial.println("//////////////////////////////////////////////////////////////////////////////////// ");
  Serial.println(" ");
  Serial.println("this is unit information mode menu, please select working process mode:");
  Serial.println("a. print unit info( auto working when changing )");
  Serial.println("b. adjust unit Arrived status");
  Serial.println("e. back to main menu");
  Serial.println("please choose mode:");
}
