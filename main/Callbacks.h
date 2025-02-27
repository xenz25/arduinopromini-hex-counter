void cbCheckButtons() {
  configButton.check();
}

void cbPreviewDisplayCounter() {
  const uint32_t PREVIEW_TIMEOUT = 7000;

  if (stateMgr.getActiveState() == STATE_WAITING && dispMgr.getDisplayState() == HIGH) {
    if (millis() - PREV_ON_TIME > PREVIEW_TIMEOUT) {
      dispMgr.turnOFF();
    }
  }
}

void cbUpdateBrightnessData() {
  uint8_t brgtData = atol(dataMgr.ACT_BRGT_DAT);
  brgtData = brgtData >= DataManager::MAX_BRGT ? DataManager::MIN_BRGT : ++brgtData;
  sprintf(dataMgr.ACT_BRGT_DAT, "%d", brgtData);
  dispMgr.setBrightness(brgtData);
  dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
}

// responsible for blinking a specific digit during counter data configuration
void cbBlinkDigitOnConfig() {
  const uint32_t DIGIT_BLINK_SPEED = 450;
  static uint32_t PREV_BLINK_TIME = 0;
  // true is visible
  static bool digitVisible = true;

  if (stateMgr.getActiveState() == STATE_CONFIG_CNTR && !IS_DIGIT_CHANGING) {
    if (millis() - PREV_BLINK_TIME > DIGIT_BLINK_SPEED) {
      if (digitVisible) {
        dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
      } else {
        dispMgr.displayText(dataMgr.ACT_CNTR_DAT, ACT_BLINK_DIGIT);
      }

      PREV_BLINK_TIME = millis();
      digitVisible = !digitVisible;
    }
  } else {
    digitVisible = true;
  }
}

void cbConfigCounterRoutine() {
  IS_DIGIT_CHANGING = true;  // enabled to disabled blinking digit

  // get the allowed character count and put all allowed character count in front of the buffer
  char copyBuffer[DataManager::COUNTER_DATA_SIZE] = "";
  uint8_t allowedCharCount = dispMgr.copyAllowedChars(dataMgr.ACT_CNTR_DAT, arrLength(dataMgr.ACT_CNTR_DAT), copyBuffer);

  // create a copy of copyBuffer padded with zeros
  char destinationBuffer[DisplayManager::MAX_SEGMENT_COUNT] = "";
  uint8_t destLength = arrLength(destinationBuffer);

  if (allowedCharCount > destLength) {
    allowedCharCount = destLength;
  }

  dispMgr.addZerosToFront(destLength - allowedCharCount, copyBuffer, destinationBuffer, destLength);

  // get the next character based on previous character
  uint8_t activeDigitIndex = invertIndex(destLength, ACT_BLINK_DIGIT);
  char let = destinationBuffer[activeDigitIndex];
  char nextChar = dispMgr.getNextEquivalentCharacter(let);

  // replace the old character with the nextChar
  destinationBuffer[activeDigitIndex] = nextChar;

  // reassign it back and display it
  strncpy(dataMgr.ACT_CNTR_DAT, destinationBuffer, sizeof(dataMgr.ACT_CNTR_DAT));
  dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
}

void cbConfigCounterDoubleClick() {
  IS_DIGIT_CHANGING = false;  // force to set to digit blink

  if (ACT_BLINK_DIGIT >= DisplayManager::MAX_SEGMENT_COUNT - 1) {
    ACT_BLINK_DIGIT = 0;
  } else {
    ACT_BLINK_DIGIT++;
  }
}

void cbConfigCounterLongPressed() {
  char normalizedBuffer[DataManager::COUNTER_DATA_SIZE] = "";
  uint8_t allowedCharCount = dispMgr.copyAllowedChars(dataMgr.ACT_CNTR_DAT, arrLength(dataMgr.ACT_CNTR_DAT), normalizedBuffer);

  char cleanBuffer[DataManager::COUNTER_DATA_SIZE] = "";
  for (int i = 0; i < allowedCharCount; i++) {
    cleanBuffer[i] = normalizedBuffer[i];
  }

  // save configured counter data
  dataMgr.saveCounterData(cleanBuffer);
  ACT_BLINK_DIGIT = 0;

  // Retrieve the saved counter data
  char buffer[DataManager::COUNTER_DATA_SIZE] = "";
  dataMgr.getCounterData(buffer);

  dispMgr.showBrgtConfigLabel();
  delay(1200);

  // display it
  dispMgr.displayText(buffer);
  stateMgr.setActiveState(STATE_CONFIG_BRGT);
}

void cbObserveDigitChangeFlag() {
  uint32_t DIGIT_CHANGE_WAIT = 2000;
  static uint32_t PREV_ENABLED_TIME = 0;

  if (millis() - PREV_ENABLED_TIME > DIGIT_CHANGE_WAIT) {
    IS_DIGIT_CHANGING = false;
    PREV_ENABLED_TIME = millis();
  }
}

void cbClickHandler() {
  switch (stateMgr.getActiveState()) {
    case STATE_WAITING:
      dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
      PREV_ON_TIME = millis();
      break;
    case STATE_CONFIG_BRGT:
      cbUpdateBrightnessData();
      break;
    case STATE_CONFIG_CNTR:
      cbConfigCounterRoutine();
      break;
  }
}

void cbDoubleClickHandler() {
  switch (stateMgr.getActiveState()) {
    case STATE_CONFIG_CNTR:
      cbConfigCounterDoubleClick();
      break;
  }
}

void cbLongPressHandler() {
  switch (stateMgr.getActiveState()) {
    case STATE_WAITING:
      dispMgr.showConfigLabel();
      delay(1200);
      stateMgr.setActiveState(STATE_CONFIG_CNTR);
      dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
      break;
    case STATE_CONFIG_CNTR:
      cbConfigCounterLongPressed();
      break;
    case STATE_CONFIG_BRGT:
      dataMgr.saveBrightnessData(dataMgr.ACT_BRGT_DAT);
      dispMgr.showDoneConfigLabel();
      delay(1200);
      dispMgr.displayText(dataMgr.ACT_CNTR_DAT);
      stateMgr.setActiveState(STATE_WAITING);
      PREV_ON_TIME = millis();
      break;
  }
}