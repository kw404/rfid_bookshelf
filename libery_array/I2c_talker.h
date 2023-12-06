#ifndef I2C_TALKER_H
#define I2C_TALKER_H

#include <Arduino.h>
#include <Wire.h>
#define SLAVE_ADDRESS 0x12
#define DATA_SIZE 8

void requestBookData(char *bookID, char *cabinetID);
void talker_report(int mode, char *receivedData) ;

#endif