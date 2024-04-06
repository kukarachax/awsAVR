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

void lcdPrintDtTm(const bool DorT, const byte x, const byte y) {
  lcd.setCursor(x,y);
  if (!DorT) lcd.print(rtc.getTimeString().c_str());
  else lcd.print(rtc.getDateString().c_str());
}

byte onBtn(byte* ptr, const int step = 0) {
  if (!digitalRead(btn_ok)) {
    tone(buzzer_pin, 3000, 200);
    delay(150);
    return 1;
  }
  else if (!digitalRead(btn_cancel)) {
    tone(buzzer_pin, 1500, 200);
    delay(150);
    return 2;
  }
  else if (!digitalRead(btn_up)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    *ptr = *ptr - step;
    return 3; 
  }
  else if (!digitalRead(btn_down)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    *ptr = *ptr + step;
    return 3; 
  }
  
  return 0;
}

byte onBtn(uint32_t* ptr, const int step = 0) {
  if (!digitalRead(btn_ok)) {
    tone(buzzer_pin, 3000, 200);
    delay(150);
    return 1;
  }
  else if (!digitalRead(btn_cancel)) {
    tone(buzzer_pin, 1500, 200);
    delay(150);
    return 2;
  }
  else if (!digitalRead(btn_up)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    *ptr = *ptr - step;
    return 3; 
  }
  else if (!digitalRead(btn_down)) {
    tone(buzzer_pin, 3000, 100);
    delay(150);
    *ptr = *ptr + step;
    return 3; 
  }
  
  return 0;
}

void lcdPrintPointer() {
  lcd.home();
  lcd.print(">");
}

void buildmenu(const bool setCh, const byte NumOfStrs, const char** strs, void (**funs)(), void (*backFunc)()) {
  byte ptr = 0;
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
        ptr = constrain(ptr, 0, NumOfStrs);
        if (ptr != NumOfStrs) lcdPrint(true, strs[ptr], strs[ptr+1]);
        else lcdPrint(true, strs[ptr], "");
        lcdPrintPointer();
        break;
    }
  }
}

void Menulogo() {
  lcdPrint(true, "Auto Watering", ">|Settings");
  byte ptr = 0;
  while (true) {
    if (onBtn(&ptr) == 1) MenuMainPage();
    delay(0);
  }
}

void MenuMainPage() {
  const char* strs[] = {" |Channel 1", " |Channel 2", " |Debug"};
  void (*funs[])() = {MenuChannel, MenuChannel, MenuDebug};
  buildmenu(true, 2, strs, funs, &Menulogo);
}

void MenuChannel() {
  void (*funs[])() = {MenuChannel_mode, MenuChannel_dur, MenuChannel_time, MenuChannel_force};
  if (ChannelMode[menuChannel] == 3) {
    const char* strs[] = {" |Mode", " |Duration", " |Interval", " |Force test"};
    buildmenu(false, 4, strs, funs, &MenuMainPage);
  }
  else {
    const char* strs[] = {" |Mode", " |Duration", " |Start Time", " |Force test"};
    buildmenu(false, 4, strs, funs, &MenuMainPage);
  }
  
  
}

void MenuDebug() {
  unsigned long timerUpd = millis();
  byte ptr = 0;
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
        lcdPrint(true, "CH1:", "Date:");
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      }
    }

    if (onBtn(&ptr) == 3) {
      ptr = constrain(ptr, 0, 4);
    }
    else if (onBtn(&ptr) == 2) {
      MenuMainPage();
      return;
    }
  }

}


void buildmenuChng(const uint16_t step, const uint16_t minVal, const uint32_t maxVal, const char name[7], const char* unit, const uint16_t multipler, uint32_t** arg, void (*backFunc)()) {
  uint32_t ptr = 0;
  lcdPrint(true, name, unit);

  while (true) {
    switch (onBtn(&ptr, step)) {
      case 0:
        break;
      case 1:
        arg[menuChannel] = ptr * multipler;
        backFunc();
        return;
        break;
      case 2:
        backFunc();
        return;
        break;
      case 3:
        ptr = constrain(ptr, minVal, maxVal);
        lcd.setCursor(8, 0);
        lcd.print(ptr);
        break;
    }
  }
}

void MenuChannel_mode() {
  buildmenuChng(1, 0, 3, "Mode:", "Watch Debug", 1, &ChannelMode, &MenuChannel);
}

void MenuChannel_dur() {
  buildmenuChng(5, 1, 300, "Dur-n:", "In minutes", 60000, &ChannelTimeout, &MenuChannel);
}

void MenuChannel_time() {
  switch (ChannelMode[menuChannel]) {
  case 1:
    const char* strs1[] = {" |Hour", " |Minute"};
    void (*funs1[])() = {MenuChannel_timeH, MenuChannel_timeM};
    buildmenu(false, 1, strs1, funs1, &MenuChannel);
    break;
  case 2:
    const char* strs2[] = {" |Week day", " |Hour", " |Minute"};
    void (*funs2[])() = {MenuChannel_timeD, MenuChannel_timeH, MenuChannel_timeM};
    buildmenu(false, 2, strs2, funs2, &MenuChannel);
    break;
  case 3:
    buildmenuChng(5, 1, 1488, "Int-l:", "In minutes", 60000, &ChannelInterval, &MenuChannel);
    break;
  }
}

void MenuChannel_timeD() {
  buildmenuChng(1, 1, 7, "Day:", "1-7 [Mon-Sun]", 1, &ChannelsD, &MenuChannel_time);
}

void MenuChannel_timeH() { 
  buildmenuChng(1, 0, 23, "Hour:", "In hours", 1, &ChannelsH, &MenuChannel_time);
}

void MenuChannel_timeM() { 
  buildmenuChng(1, 0, 59, "Minute:", "In minutes", 1, &ChannelsM, &MenuChannel_time);
}  

void MenuChannel_force() {
  byte ptr = 0;
  lcdPrint(true, "Cancel to exit", "Watering...");

  relayOn(true, true, menuChannel);

  delay(150);

  while (true) {
    if (onBtn(&ptr) == 2) {
      relayOn(true, false, menuChannel);

      MenuChannel();
      return;
    }
    delay(0);
  }
}
