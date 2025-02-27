#include <Arduino.h>
#include <AceButton.h>
#include <Thread.h>
#include <ThreadController.h>
#include <TM1637Display.h>

#include "Constants.h"
#include "StateManager.h"
#include "DataManager.h"
#include "DisplayManager.h"

using namespace ace_button;

StateManager stateMgr;
DataManager dataMgr;
TM1637Display display(TM_CLK, TM_DIO);
DisplayManager dispMgr(&display, ACT_BRGT_DATA);
AceButton configButton(CONFIG_BTN);

// Thread Pool Manager
ThreadController thdController = ThreadController();
Thread thdCheckButton = Thread();
Thread thdCheckPreviewState = Thread();
Thread thdBlinkDigitOnConfig = Thread();
Thread thdObserveDigitChangeFlag = Thread();

// Should be included at this point
#include "Callbacks.h"
#include "Setup.h"

// Known Issues:
// Sometimes the buffer when saving overflows causes the system to restart

void setup() {
  Serial.begin(9600);
  delay(2000);

  stateMgr.begin();
  initializeDisplay();
  initializeDataStore();

  // Loading Animation
  dispMgr.showLoadingAnimation();

  initilizeDisplayBrightness();
  initilizeCounterData();
  initializeConfigButton();

  // Configure Threads
  initializedThreads();

  // Set state manager to waiting
  stateMgr.setActiveState(STATE_WAITING);
  PREV_ON_TIME = millis();
}

void loop() {
  thdController.run();
}

// config button event handler
void handleEvent(AceButton*, uint8_t eventType,
                 uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventClicked:
    case AceButton::kEventReleased:
      cbClickHandler();
      break;
    case AceButton::kEventDoubleClicked:
      cbDoubleClickHandler();
      break;
    case AceButton::kEventLongPressed:
      cbLongPressHandler();
      break;
  }
}
