// config button event handler
void handleEvent(AceButton*, uint8_t, uint8_t);

void initializeDisplay() {
  dispMgr.clearSegments();
}

void initializeDataStore() {
  // Check SD Card if present
  if (!dataMgr.begin()) {
    dispMgr.displayError(SEGMENT_HEX[1]);
    while (1)
      ;
  }

  // Check if SD Card files are ready
  if (!dataMgr.initializedFiles()) {
    dispMgr.displayError(SEGMENT_HEX[2]);
    while (1)
      ;
  }
}

void initilizeDisplayBrightness() {
  // Set brightness based on the config file
  char brgtData[DataManager::BRIGHT_DATA_SIZE];
  dataMgr.getBrightnessData(brgtData);
  ACT_BRGT_DATA = atol(dataMgr.ACT_BRGT_DAT);
  dispMgr.setBrightness(ACT_BRGT_DATA);
}

void initilizeCounterData() {
  // Read stored counter data and read it
  char counterData[DataManager::COUNTER_DATA_SIZE];
  dataMgr.getCounterData(counterData);
  dispMgr.displayText(counterData);
}

void initializeConfigButton() {
  pinMode(CONFIG_BTN, INPUT_PULLUP);
  ButtonConfig* buttonConfig = configButton.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
}

// Configure threads here!
void initializedThreads() {
  thdCheckButton.onRun(cbCheckButtons);
  thdCheckButton.setInterval(4);

  thdCheckPreviewState.onRun(cbPreviewDisplayCounter);
  thdCheckPreviewState.setInterval(1750);

  thdBlinkDigitOnConfig.onRun(cbBlinkDigitOnConfig);
  thdBlinkDigitOnConfig.setInterval(100);

  thdObserveDigitChangeFlag.onRun(cbObserveDigitChangeFlag);
  thdObserveDigitChangeFlag.setInterval(100);

  thdController.add(&thdCheckButton);
  thdController.add(&thdCheckPreviewState);
  thdController.add(&thdBlinkDigitOnConfig);
  thdController.add(&thdObserveDigitChangeFlag);
}
