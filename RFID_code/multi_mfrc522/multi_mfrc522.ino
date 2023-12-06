#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN1 2 // 模块1的SDA引脚
#define RST_PIN1 3 // 模块1的RST引脚
#define SS_PIN2 4 // 模块2的SDA引脚
#define RST_PIN2 5 // 模块2的RST引脚
#define SS_PIN3 6 // 模块3的SDA引脚
#define RST_PIN3 7 // 模块3的RST引脚
#define SS_PIN4 8 // 模块4的SDA引脚
#define RST_PIN4 9 // 模块4的RST引脚
// #define SS_PIN5 A0 // 模块5的SDA引脚
// #define RST_PIN5 A1 // 模块5的RST引脚

MFRC522 mfrc522_1(SS_PIN1, RST_PIN1);
MFRC522 mfrc522_2(SS_PIN2, RST_PIN2);
MFRC522 mfrc522_3(SS_PIN3, RST_PIN3);
MFRC522 mfrc522_4(SS_PIN4, RST_PIN4);
// MFRC522 mfrc522_5(SS_PIN5, RST_PIN5);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  mfrc522_1.PCD_Init();
  mfrc522_1.PCD_DumpVersionToSerial();
  mfrc522_2.PCD_Init();
  mfrc522_2.PCD_DumpVersionToSerial();
  mfrc522_3.PCD_Init();
  mfrc522_3.PCD_DumpVersionToSerial();
  mfrc522_4.PCD_Init();
  mfrc522_4.PCD_DumpVersionToSerial();
  // mfrc522_5.PCD_Init();
  // mfrc522_5.PCD_DumpVersionToSerial();
  
}

void loop() {
  // 检测模块1
  if (mfrc522_1.PICC_IsNewCardPresent() && mfrc522_1.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_1.uid);
    delay(10); // 延迟0.01秒
    mfrc522_1.PICC_HaltA();
    mfrc522_1.PCD_StopCrypto1();
  }

  // 检测模块2
  if (mfrc522_2.PICC_IsNewCardPresent() && mfrc522_2.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_2.uid);
    delay(10); // 延迟0.01秒
    mfrc522_2.PICC_HaltA();
    mfrc522_2.PCD_StopCrypto1();
  }
  
  // 检测模块3
  if (mfrc522_3.PICC_IsNewCardPresent() && mfrc522_3.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_3.uid);
    delay(10); // 延迟0.01秒
    mfrc522_3.PICC_HaltA();
    mfrc522_3.PCD_StopCrypto1();
  }
  
  // 检测模块4
  if (mfrc522_4.PICC_IsNewCardPresent() && mfrc522_4.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_4.uid);
    delay(10); // 延迟0.01秒
    mfrc522_4.PICC_HaltA();
    mfrc522_4.PCD_StopCrypto1();
  }
  
  // 检测模块5
  // if (mfrc522_5.PICC_IsNewCardPresent() && mfrc522_5.PICC_ReadCardSerial()) {
  //   printCardSerial(mfrc522_5.uid);
  //   delay(10); // 延迟0.01秒
  //   mfrc522_5.PICC_HaltA();
  //   mfrc522_5.PCD_StopCrypto1();
  // }
}

void printCardSerial(MFRC522::Uid uid) {
  // Serial.print("UID Value: ");
  for (byte i = 0; i < uid.size; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? " 0" : "");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
}
