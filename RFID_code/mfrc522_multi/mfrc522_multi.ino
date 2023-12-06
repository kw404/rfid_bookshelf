#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN1 10 // 模块1的SDA引脚
#define RST_PIN1 9 // 模块1的RST引脚
#define SS_PIN2 3 // 模块2的SDA引脚
#define RST_PIN2 8 // 模块2的RST引脚

MFRC522 mfrc522_1(SS_PIN1, RST_PIN1);
MFRC522 mfrc522_2(SS_PIN2, RST_PIN2);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522_1.PCD_Init();
  mfrc522_1.PCD_DumpVersionToSerial();
  mfrc522_2.PCD_Init();
  mfrc522_2.PCD_DumpVersionToSerial();
}

void loop() {
  // 检测模块1
  if (mfrc522_1.PICC_IsNewCardPresent() && mfrc522_1.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_1.uid);
    delay(100);
    mfrc522_1.PICC_HaltA();
    mfrc522_1.PCD_StopCrypto1();
  }

  // 检测模块2
  if (mfrc522_2.PICC_IsNewCardPresent() && mfrc522_2.PICC_ReadCardSerial()) {
    printCardSerial(mfrc522_2.uid);
    delay(100);
    mfrc522_2.PICC_HaltA();
    mfrc522_2.PCD_StopCrypto1();
  }
}

void printCardSerial(MFRC522::Uid uid) {
  for (byte i = 0; i < uid.size; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? " 0" :"");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
}
