#include <SPI.h>
#include <SD.h>

#include "Utils.h"

class DataManager {
private:
  uint8_t _CSPIN = SD_CHIP_SELECT_PIN;

  // file filename
  char _FF_COUNTER_DATA[17] = "CNTRDAT.TXT";
  char _FF_BRIGHTNESS_DATA[20] = "BRGTDAT.TXT";

  void _getFileContent(char* data, char* fileName, uint8_t size) {
    File ff = SD.open(fileName);
    uint8_t i = 0;

    if (ff) {
      while (ff.available() && i < size) {
        data[i] = ff.read();
        i++;
      }
      ff.close();
    } else {
      data[i] = '0';
    }
  }

  bool _writeToFile(char* fileName, char* data) {
    File ff = SD.open(fileName, O_CREAT | O_WRITE | O_TRUNC);

    if (ff) {
      ff.write(data);
      ff.close();
      return true;
    }

    return false;
  }

  void _writeToBuffer(char* buffer, const char* data) {
    strncpy(buffer, data, sizeof(buffer));
  }

  void _cleanBuffer(char* buffer, uint8_t bufferSize) {
    memset(buffer, '\0', bufferSize);
  }

public:
  // Data Sizes
  static const uint8_t COUNTER_DATA_SIZE = 5;
  static const uint8_t BRIGHT_DATA_SIZE = 2;

  static const uint8_t MAX_BRGT = 7;
  static const uint8_t MIN_BRGT = 1;

  char ACT_CNTR_DAT[DataManager::COUNTER_DATA_SIZE] = "";
  char ACT_BRGT_DAT[DataManager::BRIGHT_DATA_SIZE] = "";

  // Intializing SD Card
  bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN) {
    this->_CSPIN = csPin;
    return SD.begin(csPin);
  }

  // Initialized SD Card Files
  bool initializedFiles() {
    bool case1 = true;
    bool case2 = true;

    // Data defaults
    char _CNTR_DEF[DataManager::COUNTER_DATA_SIZE] = "0000";
    char _BRGT_DEF[DataManager::BRIGHT_DATA_SIZE] = "2";

    if (!SD.exists(this->_FF_COUNTER_DATA)) {
      case1 = this->_writeToFile(
        this->_FF_COUNTER_DATA, _CNTR_DEF);
    }

    if (!SD.exists(this->_FF_BRIGHTNESS_DATA)) {
      case2 = this->_writeToFile(
        this->_FF_BRIGHTNESS_DATA, _BRGT_DEF);
    }

    return case1 && case2;
  }

  void overwrite(char* buffer, const char* data) {
    this->_writeToBuffer(buffer, data);
  }

  // Gets the counter data
  void getCounterData(char* data, uint8_t size = DataManager::COUNTER_DATA_SIZE) {
    this->_cleanBuffer(data, size);
    this->_getFileContent(data, this->_FF_COUNTER_DATA, DataManager::COUNTER_DATA_SIZE);
    strcpy(this->ACT_CNTR_DAT, data);
  }

  // Gets the brightness data
  void getBrightnessData(char* data, uint8_t size = DataManager::BRIGHT_DATA_SIZE) {
    this->_cleanBuffer(data, size);
    this->_getFileContent(data, this->_FF_BRIGHTNESS_DATA, size);
    strcpy(this->ACT_BRGT_DAT, data);
  }

  // save data to counterData file
  bool saveCounterData(char* data) {
    return this->_writeToFile(this->_FF_COUNTER_DATA, data);
  }

  // save data to brightnessData file
  bool saveBrightnessData(char* data) {
    return this->_writeToFile(this->_FF_BRIGHTNESS_DATA, data);
  }
};