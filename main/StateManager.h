enum SYS_STATE {
  STATE_INIT,
  STATE_WAITING,
  STATE_CONFIG_BRGT,
  STATE_CONFIG_CNTR
};

class StateManager {
private:
  SYS_STATE ACTIVE_STATE;

public:
  void begin() {
    this->ACTIVE_STATE = STATE_INIT;
  }

  SYS_STATE getActiveState() {
    return this->ACTIVE_STATE;
  }

  void setActiveState(SYS_STATE state) {
    this->ACTIVE_STATE = state;
  }
};