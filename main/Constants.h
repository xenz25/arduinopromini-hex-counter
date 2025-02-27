#define TM_CLK 2
#define TM_DIO 3

uint8_t ACT_BRGT_DATA = 0x02;

const int CONFIG_BTN = 4;

// Display Preview Related variables
uint32_t PREV_ON_TIME = 0;

// Display Blink on Counter Data Config variables
uint8_t ACT_BLINK_DIGIT = 0;
// created to prevent blinking during updating counter digits
bool IS_DIGIT_CHANGING = false;