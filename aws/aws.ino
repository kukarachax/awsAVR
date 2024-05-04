//#define ERASE_EEPROM
//#define RESET_TIME
#define ENABLE_GERCON
#define SEC_ADD 10

#define btn_ok 7
#define btn_down 5
#define btn_up 4
#define btn_cancel 6
#define water_sensor_pin 8
#define buzzer_pin 11
#define relay1_pin 2
#define relay2_pin 3

#define RELAY_ENABLED 0
#define RELAY_DISABLED 1

#define LCD_Address 0x27
#define LCD_Cols 16
#define LCD_Rows 2
#define LCD_TimerOff 30000

#include "LiquidCrystal_I2C.h"
#include "EEPROM.h"
#include "microDS3231.h"
#include "GyverTimer.h"

#ifdef RESET_TIME
  #include <buildTime.h>
#endif

uint8_t ChannelMode[2] = {0,0};
uint8_t ChannelsD[2] = {0,0};
uint8_t ChannelsH[2] = {0,0};
uint8_t ChannelsM[2] = {0,0};
uint8_t ChannelByDays1[2] = {0,0};
uint8_t ChannelByDays2[2] = {0,0};
uint8_t ChannelByDays3[2] = {0,0};
uint8_t ChannelByDays4[2] = {0,0};
uint8_t ChannelByDays5[2] = {0,0};
uint8_t ChannelByDays6[2] = {0,0};
uint8_t ChannelByDays7[2] = {0,0};

// uint32_t ChannelInterval[2] = {0,0};
// uint32_t ChannelMillis[2] = {0,0};

uint32_t ChannelTimeout[2] = {1,1};
uint32_t lcdTimerOff = 0;

uint8_t menuChannel = 0;
uint8_t watering[2] = {0, 0};
bool dispOnOff = true;

MicroDS3231 rtc;
LiquidCrystal_I2C lcd(LCD_Address, LCD_Cols, LCD_Rows);
GTimer TimerChannel1(MS);
GTimer TimerChannel2(MS);

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

  digitalWrite(relay1_pin, RELAY_DISABLED);
  digitalWrite(relay2_pin, RELAY_DISABLED);

  #ifdef ERASE_EEPROM
    updEeprom();
  #endif

  if (EEPROM.read(0) != 123) updEeprom();
  else getEeprom();

  rtcBegin();
  setTimeAuto();
}

void loop() {
  Menulogo();
}

void updEeprom() {
  EEPROM.put(0, 123);

  EEPROM.put(2, ChannelMode[0]);
  EEPROM.put(4, ChannelMode[1]);

  EEPROM.put(6, ChannelsD[0]);
  EEPROM.put(8, ChannelsD[1]);

  EEPROM.put(10, ChannelsH[0]);
  EEPROM.put(12, ChannelsH[1]);

  EEPROM.put(14, ChannelsM[0]);
  EEPROM.put(16, ChannelsM[1]);

  EEPROM.put(18, ChannelByDays1[0]);
  EEPROM.put(20, ChannelByDays1[1]);
  EEPROM.put(22, ChannelByDays2[0]);
  EEPROM.put(24, ChannelByDays2[1]);
  EEPROM.put(26, ChannelByDays3[0]);
  EEPROM.put(28, ChannelByDays3[1]);
  EEPROM.put(30, ChannelByDays4[0]);
  EEPROM.put(32, ChannelByDays4[1]);
  EEPROM.put(34, ChannelByDays5[0]);
  EEPROM.put(36, ChannelByDays5[1]);
  EEPROM.put(38, ChannelByDays6[0]);
  EEPROM.put(40, ChannelByDays6[1]);
  EEPROM.put(42, ChannelByDays7[0]);
  EEPROM.put(44, ChannelByDays7[1]);

  EEPROM.put(54, ChannelTimeout[0]);
  EEPROM.put(58, ChannelTimeout[1]);
}

void getEeprom() { 
  EEPROM.get(2, ChannelMode[0]);
  EEPROM.get(4, ChannelMode[1]);

  EEPROM.get(6, ChannelsD[0]);
  EEPROM.get(8, ChannelsD[1]);

  EEPROM.get(10, ChannelsH[0]);
  EEPROM.get(12, ChannelsH[1]);

  EEPROM.get(14, ChannelsM[0]);
  EEPROM.get(16, ChannelsM[1]);

  EEPROM.get(18, ChannelByDays1[0]);
  EEPROM.get(20, ChannelByDays1[1]);
  EEPROM.get(22, ChannelByDays2[0]);
  EEPROM.get(24, ChannelByDays2[1]);
  EEPROM.get(26, ChannelByDays3[0]);
  EEPROM.get(28, ChannelByDays3[1]);
  EEPROM.get(30, ChannelByDays4[0]);
  EEPROM.get(32, ChannelByDays4[1]);
  EEPROM.get(34, ChannelByDays5[0]);
  EEPROM.get(36, ChannelByDays5[1]);
  EEPROM.get(38, ChannelByDays6[0]);
  EEPROM.get(40, ChannelByDays6[1]);
  EEPROM.get(42, ChannelByDays7[0]);
  EEPROM.get(44, ChannelByDays7[1]);

  EEPROM.get(54, ChannelTimeout[0]);
  EEPROM.get(58, ChannelTimeout[1]);

}