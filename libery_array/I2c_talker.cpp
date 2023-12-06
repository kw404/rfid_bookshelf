#include "I2c_talker.h"

void requestBookData(char *bookID, char *cabinetID) {
  talker_report(1, bookID);
  talker_report(2, cabinetID);
}

void talker_report(int mode, char *receivedData) {
  int dataIndex = 0;
  //chang mode
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(mode);
  delay(40);
  Wire.endTransmission();

  //reqest data
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.requestFrom(SLAVE_ADDRESS, DATA_SIZE);

  while (Wire.available()) {
    if (dataIndex < DATA_SIZE) {
      receivedData[dataIndex] = (char)Wire.read();
      dataIndex++;
    }else{
      Wire.read();
    }
  }

  Wire.endTransmission();
}
