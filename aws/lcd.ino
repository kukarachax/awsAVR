void lcdPrint(const bool clr, const char str1[17], const char str2[17]) {
  lcd.backlight();
  if (clr) {
    lcd.clear();
  }
  lcd.home();
  lcd.print(str1);

  lcd.setCursor(0,1);
  lcd.print(str2);
}
void lcdPrintCoord(const bool clr, const uint8_t x, const uint8_t y, const char* str) {
  if (clr) lcd.clear();
  lcd.backlight();
  lcd.setCursor(x, y);
  lcd.print(str);
}
void lcdPrintDtTm(const bool DorT, const uint8_t x, const uint8_t y) {
  lcd.setCursor(x,y);
  if (!DorT) lcd.print(rtc.getTimeString().c_str());
  else lcd.print(rtc.getDateString().c_str());
}
void lcdPrintPointer() {
  lcd.home();
  lcd.print(">");
}
void lcdPrintWatering() {
  uint8_t waterON[] PROGMEM = {
    B00000,
    B00100,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
    B00000
  };
  uint8_t waterOff[] PROGMEM = {
    B00000,
    B00100,
    B01110,
    B10111,
    B11011,
    B11101,
    B01110,
    B00000
  };
  uint8_t EmptyBarrel[] PROGMEM = {
    B11111,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111
  };
  uint8_t FullBarrel[] PROGMEM = {
    B11111,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };
  lcd.createChar(0, waterON);
  lcd.createChar(1, waterOff);
  lcd.createChar(2, EmptyBarrel);
  lcd.createChar(3, FullBarrel);
  
  lcd.setCursor(15, 0);
  if (isGerconEnabled()) lcd.write(3);
  else lcd.write(2);

  for (uint8_t i = 0; i < 2; i++) {
    lcd.setCursor(11 + i * 2, 0);
    if (watering[i] == 1) lcd.write(0);
    else lcd.write(1);
  }
}

uint16_t onBtn(uint16_t* ptr, const uint16_t step = 1, const bool invert = false) {
  relayTick();
  
  if (!digitalRead(btn_ok)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    return 1;
  }
  else if (!digitalRead(btn_cancel)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    return 2;
  }
  else if (!digitalRead(btn_up)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    if (!invert) *ptr = *ptr - step;
    else *ptr = *ptr + step;
    return 3; 
  }
  else if (!digitalRead(btn_down)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    if (!invert) *ptr = *ptr + step;
    else *ptr = *ptr - step;
    return 3; 
  }
  
  return 0;
}

void buildmenu(const bool setCh, const uint8_t NumOfStrs, const char** strs, void (**funs)(), void (*backFunc)()) { //SetCh, NumStrs, Strs, Funs, BackFun
  uint16_t ptr = 0;
  lcdPrint(true, strs[0], strs[1]);
  lcdPrintPointer();

  while (true) {
    switch (onBtn(&ptr)) {
      case 0:
        break;
      case 1:
        if (setCh && ptr < 2) menuChannel = ptr;
        funs[ptr]();
        return;
        break;
      case 2:
        backFunc();
        return;
        break;
      case 3:
        ptr = constr(ptr, 0, NumOfStrs);
        if (ptr != NumOfStrs) lcdPrint(true, strs[ptr], strs[ptr+1]);
        else lcdPrint(true, strs[ptr], "");
        lcdPrintPointer();
        break;
    }
  }
}
uint32_t buildmenuChng(const uint16_t step, const uint16_t minVal, const uint32_t maxVal, const char name[7], const uint8_t xPos, const char* unit, const uint32_t multipler, uint32_t arg) { //Step, Min, Max, Name, Pos, Unit, Multipler, Arg
  uint16_t ptr = arg / multipler;
  lcdPrint(true, name, unit);
  lcd.setCursor(xPos, 0);
  lcd.print(ptr);

  while (true) {
    switch (onBtn(&ptr, step, true)) {
      case 0:
        break;
      case 1:
        arg = ptr * multipler;
        return arg;
        break;
      case 2:
        return arg;
        break;
      case 3:
        ptr = constr(ptr, minVal, maxVal);
        
        for (uint8_t i = xPos; i < 15; i++) {
          lcd.setCursor(i, 0);
          lcd.print(" ");
        }

        lcd.setCursor(xPos, 0);
        lcd.print(ptr);
        break;
    }
  }
  return arg;
}

void Menulogo() {
  startLCDTimer();
  lcdPrint(true, "AWS", ">|Settings");
  uint16_t ptr = 0;

  while (true) {
    lcdPrintWatering();

    if (LCDTimerTick(LCD_TimerOff)) MenuLogoDispOff();

    uint8_t i = onBtn(&ptr);

    if (i > 1) startLCDTimer();
    if (i == 1) MenuMainPage();
    delay(0);
  }
}

void MenuLogoDispOff() {
  lcd.noBacklight();
  lcd.clear();
  
  uint16_t i;
  while (!onBtn(&i) != 0) {
    relayTick();
  }

  Menulogo();
}

void MenuMainPage() {
  const char* strs[] = {" |Channel 1", " |Channel 2", " |Debug"};
  void (*funs[])() = {MenuChannel, MenuChannel, MenuDebug};
  buildmenu(true, 2, strs, funs, &Menulogo);
}

void MenuChannel() {
  if (ChannelMode[menuChannel] == 0) {
    void (*funs[])() = {MenuChannel_mode, MenuChannel_dur, MenuChannel_force};
    const char* strs[] = {" |Mode", " |Duration", " |Force test"};
    buildmenu(false, 2, strs, funs, &MenuMainPage);
  }
  else {
    void (*funs[])() = {MenuChannel_mode, MenuChannel_dur, MenuChannel_time, MenuChannel_force};
    const char* strs[] = {" |Mode", " |Duration", " |Start Time", " |Force test"};
    buildmenu(false, 3, strs, funs, &MenuMainPage);
  }
}

String getModeName(const uint8_t chnl) {
  switch (ChannelMode[chnl]) {
  case 0:
    return "Off";
    break;
  case 1:
    return "Interval";
    break;
  }
}
String getDays(const uint8_t chnl) { 
  String str = "";

  if (ChannelByDays1[chnl]) str += "M ";
  else str += "m ";
  if (ChannelByDays2[chnl]) str += "T ";
  else str += "t ";
  if (ChannelByDays3[chnl]) str += "W ";
  else str += "w ";
  if (ChannelByDays4[chnl]) str += "T ";
  else str += "t ";
  if (ChannelByDays5[chnl]) str += "F ";
  else str += "f ";
  if (ChannelByDays6[chnl]) str += "S ";
  else str += "s ";
  if (ChannelByDays7[chnl]) str += "S";
  else str += "s";

  return str;
}
void lcdPrintStartTime(const uint8_t chnl) {
  switch (ChannelMode[chnl]) {
  case 0:
    lcdPrint(true, " : Off", "No information");
    lcdPrintCoord(false, 0, 0, String(chnl+1).c_str());
    break;
  case 1: 
    lcdPrint(true, " D:", " HM:   :");
    lcdPrintCoord(false, 0, 0, String(chnl+1).c_str());
    lcdPrintCoord(false, 0, 1, String(chnl+1).c_str());

    lcdPrintCoord(false, 3, 0, getDays(chnl).c_str());
    lcdPrintCoord(false, 5, 1, String(ChannelsH[chnl]).c_str());
    lcdPrintCoord(false, 8, 1, String(ChannelsM[chnl]).c_str());
    break;
  }
}
void MenuDebug() {
  uint32_t timerUpd = millis();
  uint16_t ptr = 0;
  while (true) {
    if (millis() - timerUpd > 1000) {
    timerUpd = millis();
    switch (ptr) {
      case 0: 
        lcdPrint(true, "Time:", "Date:");
        lcdPrintDtTm(true, 6, 1);
        lcdPrintDtTm(false, 6, 0);
        break;
      case 1:
        lcdPrint(true, "1:", "2:");
        lcdPrintCoord(false, 3, 0, getModeName(0).c_str());
        lcdPrintCoord(false, 3, 1, getModeName(1).c_str());
        break;
      case 2:
        lcdPrintStartTime(0);
        break;
      case 3: 
        lcdPrintStartTime(1);
        break;
      case 4:
        lcdPrint(true, "1:", "2:");
        lcdPrintCoord(false, 3, 0, String(ChannelTimeout[0] / 60000).c_str());
        lcdPrintCoord(false, 3, 1, String(ChannelTimeout[1] / 60000).c_str());
        break;
      case 5:
        lcdPrint(true, "Gercon: ", "");
        lcdPrintCoord(false, 8, 0, String(isGerconEnabled()).c_str());
        break;
      }
    }

    if (onBtn(&ptr) == 3) {
      ptr = constr(ptr, 0, 5);
    }
    else if (onBtn(&ptr) == 2) {
      MenuMainPage();
      return;
    }
  }

}

void MenuChannel_mode() {
  ChannelMode[menuChannel] = buildmenuChng(1, 0, 1, "Mode:", 6, "Watch Debug..", 1, ChannelMode[menuChannel]);
  updEeprom();
  MenuChannel();
}

void MenuChannel_time() {
  const char* strs2[] = {" |Week day", " |Hour", " |Minute"};
  void (*funs2[])() = {MenuChannel_mode_bydays, MenuChannel_timeH, MenuChannel_timeM};
  buildmenu(false, 2, strs2, funs2, &MenuChannel);
}
void MenuChannel_timeH() { 
  ChannelsH[menuChannel] = buildmenuChng(1, 0, 23, "Hour:", 6, "In hours", 1, ChannelsH[menuChannel]);
  updEeprom();
  MenuChannel_time();
}
void MenuChannel_timeM() { 
  ChannelsM[menuChannel] = buildmenuChng(1, 0, 59, "Minute:", 8, "In minutes", 1, ChannelsM[menuChannel]);
  updEeprom();
  MenuChannel_time();
}  
void MenuChannel_mode_bydays() {
  const char* strs[] = {" |Monday", " |Tuesday", " |Wednesday", " |Thursday", " |Friday", " |Staruday", " |Sunday"};
  void (*funs[])() = {MenuChannel_mode_bydaysMo, MenuChannel_mode_bydaysTu, MenuChannel_mode_bydaysWe, MenuChannel_mode_bydaysTh, MenuChannel_mode_bydaysFr, MenuChannel_mode_bydaysSa, MenuChannel_mode_bydaysSu};
  buildmenu(false, 6, strs, funs, &MenuChannel);
}
void MenuChannel_mode_bydaysMo() {
  ChannelByDays1[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays1[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysTu() {
  ChannelByDays2[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays2[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysWe() {
  ChannelByDays3[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays3[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysTh() {
  ChannelByDays4[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays4[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysFr() {
  ChannelByDays5[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays5[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysSa() {
  ChannelByDays6[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays6[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}
void MenuChannel_mode_bydaysSu() {
  ChannelByDays7[menuChannel] = buildmenuChng(1, 0, 1, "Turn:", 6, "0 - Off; 1 - On", 1, ChannelByDays7[menuChannel]);
  updEeprom();
  MenuChannel_mode_bydays();
}

void MenuChannel_dur() {
  ChannelTimeout[menuChannel] = buildmenuChng(5, 0, 300, "Dur-n:", 7, "In minutes", 60000, ChannelTimeout[menuChannel]);
  updEeprom();
  MenuChannel();
}

void MenuChannel_force() {
  if (watering[menuChannel] == 0) { 
    uint16_t ptr = 0;

    lcdPrint(true, "Cancel to exit", "Watering...");
    relaySendSignal(RELAY_ENABLED, menuChannel);
    delay(150);

    while (true) {
      if (onBtn(&ptr) == 2 || !isGerconEnabled()) {
        relaySendSignal(RELAY_DISABLED, menuChannel);

        MenuChannel();
        return;
      }
      delay(0);
    }
  }
}

void startLCDTimer() {
  lcdTimerOff = millis();
}

bool LCDTimerTick(const uint32_t timeOff) {
  if (millis() - lcdTimerOff > timeOff) return true;
  return false;
}

int32_t constr(int32_t x, const int32_t a, const int32_t b) {
  if (x < a) return b;
  if (x >= a && x <= b) return x;
  if (x > b) return a;
}