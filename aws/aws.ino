//#define ERASE_EEPROM
#define RESET_TIME

#define btn_ok 7
#define btn_down 5
#define btn_up 4
#define btn_cancel 6
#define water_sensor_pin 8
#define buzzer_pin 11
#define relay1_pin 2
#define relay2_pin 3

#define RELAY_HIGH 0
#define RELAY_LOW 1

#define LCD_Address 0x27
#define LCD_Cols 16
#define LCD_Rows 2

#include "LiquidCrystal_I2C.h"
#include "EEPROM.h"
#include "microDS3231.h"

#ifdef RESET_TIME
  #include <buildTime.h>
#endif

uint8_t* ChannelMode[2] = {0,0};
uint8_t* ChannelsD[2] = {0,0};
uint8_t* ChannelsH[2] = {0,0};
uint8_t* ChannelsM[2] = {0,0};

uint32_t* ChannelInterval[2] = {0,0};
uint32_t ChannelMillis[2] = {0,0};

uint32_t* ChannelTimeout[2] = {0,0};

byte menuChannel = 0;

MicroDS3231 rtc;
LiquidCrystal_I2C lcd(LCD_Address, LCD_Cols, LCD_Rows);

void setup() {
  Serial.begin(115200);
  lcd.init();

  pinMode(btn_ok, INPUT_PULLUP);
  pinMode(btn_down, INPUT_PULLUP);
  pinMode(btn_up, INPUT_PULLUP);
  pinMode(btn_cancel, INPUT_PULLUP);
  pinMode(water_sensor_pin, INPUT_PULLUP);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);

  digitalWrite(relay1_pin, RELAY_LOW);
  digitalWrite(relay2_pin, RELAY_LOW);

  rtcBegin();
}

void loop() {
  Menulogo();
  relayTick();
}