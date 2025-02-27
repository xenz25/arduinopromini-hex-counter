#include "Utils.h"

uint8_t SEGMENT_HEX[16] = { 0x3f, 0x06, 0x5b, 0X4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

class DisplayManager {
private:
  TM1637Display *display;
  const uint8_t NUM_MIN_RANGE = 48;
  const uint8_t NUM_MAX_RANGE = 57;
  const uint8_t ALPHA_MIN_RANGE = 65;
  const uint8_t ALPHA_MAX_RANGE = 70;
  const uint8_t QUESTION = 63;

  bool isNumeric(char let) {
    return let >= this->NUM_MIN_RANGE && let <= this->NUM_MAX_RANGE;
  }

  bool isAF(char let) {
    return let >= this->ALPHA_MIN_RANGE && let <= this->ALPHA_MAX_RANGE;
  }

  bool isQuestion(char let) {
    return let == 63;
  }

  void translateToSegments(char *data, uint8_t *segments, uint8_t segmentCount) {
    for (uint8_t i = 0; i < segmentCount; i++) {
      segments[i] = this->translateToSegmentHex(data[i]);
    }
  }

  uint8_t translateToSegmentHex(uint8_t data) {
    uint8_t let = toupper(data);

    if (this->isNumeric(let)) {
      // 0 - 9
      return SEGMENT_HEX[let - 48];
    } else if (this->isAF(let)) {
      // A - F or a - f
      return SEGMENT_HEX[let - 55];
    } else if (this->isQuestion(let)) {
      // QUESTION
      return SEGMENT_BLANK;
    }

    //unknown characters are set to zero
    return SEGMENT_HEX[0];
  }

  bool isAllowedChar(char let) {
    let = toupper(let);
    return this->isNumeric(let) || this->isAF(let) || this->isQuestion(let);
  }

public:
  uint8_t DISPLAY_STATE;
  uint8_t BRGT_DATA;
  const uint8_t SEGMENT_BLANK = 0x0;
  const uint8_t SEGMENT_FULL = 0x7f;
  static const uint8_t MAX_SEGMENT_COUNT = 4;

  int BLANK_SEGMENT_DISABLED = -1;
  uint8_t ERR_LBL_HEX[MAX_SEGMENT_COUNT] = { 0x79, 0x50, 0x50, 0x0 };
  uint8_t CNFG_LBL_HEX[MAX_SEGMENT_COUNT] = { 0x39, 0x3F, 0x54, 0x71 };
  uint8_t BRGT_LBL_HEX[MAX_SEGMENT_COUNT] = { 0x7C, 0x50, 0x38, 0x00 };
  uint8_t DONE_LBL_HEX[MAX_SEGMENT_COUNT] = { 0x5E, 0x5C, 0x54, 0x79 };
  const uint8_t LOADING_ANIMATION_HEX[17][4] = {
    { 0x0, 0x0, 0x0, 0x0 },
    { 0x0, 0x0, 0x0, 0x01 },
    { 0x0, 0x0, 0x01, 0x01 },
    { 0x0, 0x01, 0x01, 0x0 },
    { 0x01, 0x01, 0x0, 0x0 },
    { 0x21, 0x0, 0x0, 0x0 },
    { 0x60, 0x0, 0x0, 0x0 },
    { 0x40, 0x40, 0x0, 0x0 },
    { 0x0, 0x40, 0x40, 0x0 },
    { 0x0, 0x0, 0x40, 0x40 },
    { 0x0, 0x0, 0x0, 0x44 },
    { 0x0, 0x0, 0x0, 0x0C },
    { 0x0, 0x0, 0x08, 0x08 },
    { 0x0, 0x08, 0x08, 0x0 },
    { 0x08, 0x08, 0x0, 0x0 },
    { 0x08, 0x0, 0x0, 0x0 },
    { 0x0, 0x0, 0x0, 0x0 },
  };

  DisplayManager(TM1637Display *display, uint8_t brightness = 0x4) {
    this->display = display;
    this->setBrightness(brightness);
    this->turnON();
  }

  uint8_t getDisplayState() {
    return this->DISPLAY_STATE;
  }

  void setDisplayStateHigh() {
    this->DISPLAY_STATE = HIGH;
  }

  void setDisplayStateLow() {
    this->DISPLAY_STATE = LOW;
  }

  void setBrightness(uint8_t brightness) {
    this->BRGT_DATA = brightness;
    this->display->setBrightness(brightness);
  }

  void setDisplaySegment(uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t seg4) {
    uint8_t displaySegments[4] = { seg1, seg2, seg3, seg4 };
    this->display->setSegments(displaySegments);
  }

  void setDisplaySegment(const uint8_t *displaySegments) {
    this->display->setSegments(displaySegments);
  }

  void clearSegments() {
    this->setDisplaySegment(SEGMENT_BLANK, SEGMENT_BLANK, SEGMENT_BLANK, SEGMENT_BLANK);
  }

  void turnON() {
    this->setDisplayStateHigh();
  }

  void turnOFF() {
    this->clearSegments();
    this->setDisplayStateLow();
  }

  uint8_t countAllowedChars(char *data, uint8_t dataLength) {
    uint8_t allowedCount = 0;

    for (uint8_t i = 0; i < dataLength; i++) {
      char let = data[i];
      if (this->isAllowedChar(let)) {
        allowedCount++;
      }
    }

    return allowedCount;
  }

  uint8_t copyAllowedChars(char *data, uint8_t dataLength, char *destination) {
    uint8_t currentBufferIndex = 0;
    uint8_t allowedCount = 0;

    for (uint8_t i = 0; i < dataLength; i++) {
      char let = data[i];
      if (this->isAllowedChar(let)) {
        destination[currentBufferIndex] = let;
        currentBufferIndex++;
        allowedCount++;
      }
    }

    return allowedCount;
  }

  char getNextEquivalentCharacter(char let) {
    if (!this->isAllowedChar(let)) {
      return '0';
    }

    if (this->isNumeric(let) && let == this->NUM_MAX_RANGE) {
      // number 9 in decimal - then we must switch the char to A
      return this->ALPHA_MIN_RANGE;
    } else if (isAF(let) && let == this->ALPHA_MAX_RANGE) {
      // character 'F' in decimal - then we must switch back to number 0
      return this->NUM_MIN_RANGE;
    } else if (this->isQuestion(let)) {
      return let;
    }

    return let += 1;
  }

  // add character '0' to data array provided
  void addZerosToFront(uint8_t numberOfZeros, char *source, char *dest, uint8_t destLength) {
    if (numberOfZeros > destLength) {
      numberOfZeros = destLength;
    }

    uint8_t lastInsertedZeroIdx = -1;
    strcpy(dest, "");

    // Add the zeros
    for (uint8_t i = 0; i < numberOfZeros; i++) {
      dest[i] = this->NUM_MIN_RANGE;
      lastInsertedZeroIdx = i;
    }

    // fill the remaining space with the source data
    for (uint8_t i = 0; i < destLength - numberOfZeros; i++) {
      uint8_t nextIndex = ++lastInsertedZeroIdx;
      dest[nextIndex] = source[i];
    }
  }

  void displayError(uint8_t code) {
    ERR_LBL_HEX[MAX_SEGMENT_COUNT - 1] = code;
    this->setDisplaySegment(ERR_LBL_HEX);
    this->turnON();
  }

  void displayText(char *data, int blankSegmentNumber = -1, bool rightAsStartingDigit = true) {
    if (blankSegmentNumber >= this->BLANK_SEGMENT_DISABLED) {
      if (blankSegmentNumber > this->MAX_SEGMENT_COUNT) {
        blankSegmentNumber = this->MAX_SEGMENT_COUNT - 1;
      }

      if (rightAsStartingDigit) {
        blankSegmentNumber = invertIndex(this->MAX_SEGMENT_COUNT, blankSegmentNumber);
      }
    }

    // buffers
    char buffer[MAX_SEGMENT_COUNT] = "";
    char destinationBuffer[MAX_SEGMENT_COUNT] = "";
    uint8_t segments[MAX_SEGMENT_COUNT];

    // copy only allowed characters to front of destination buffer
    uint8_t allowedCharCount = this->copyAllowedChars(data, arrLength(buffer), buffer);
    uint8_t segmentCount = arrLength(segments);

    if (allowedCharCount > segmentCount) {
      allowedCharCount = segmentCount;
    }

    this->addZerosToFront(segmentCount - allowedCharCount, buffer, destinationBuffer, segmentCount);
    this->translateToSegments(destinationBuffer, segments, segmentCount);

    // set the segment index to a blank segment
    if (blankSegmentNumber > BLANK_SEGMENT_DISABLED) {
      segments[blankSegmentNumber] = this->SEGMENT_BLANK;
    }

    this->setDisplaySegment(segments);
    this->turnON();
  }

  void showConfigLabel() {
    this->setDisplaySegment(this->CNFG_LBL_HEX);
    this->turnON();
  }

  void showBrgtConfigLabel() {
    this->setDisplaySegment(this->BRGT_LBL_HEX);
    this->turnON();
  }

  void showDoneConfigLabel() {
    this->setDisplaySegment(this->DONE_LBL_HEX);
    this->turnON();
  }

  void showLoadingAnimation(uint32_t speed = 50, uint32_t interval = 1000, uint8_t repeat = 3) {
    uint32_t previousRepeat = 0;

    int repeatCount = 0;

    this->turnON();

    while (repeatCount <= repeat - 1) {
      uint32_t currentRepeat = millis();

      if (currentRepeat - previousRepeat >= interval) {
        repeatCount++;
        previousRepeat = currentRepeat;

        uint32_t prevSegMovement = 0;
        uint8_t curAnim = 0;

        while (true) {
          if (curAnim >= arrLength(LOADING_ANIMATION_HEX)) {
            break;
          }

          uint32_t currentTime = millis();
          if (currentTime - prevSegMovement >= speed) {
            this->setDisplaySegment(LOADING_ANIMATION_HEX[curAnim]);
            curAnim++;
            prevSegMovement = currentTime;
          }
        }
      }
    }
  }
};