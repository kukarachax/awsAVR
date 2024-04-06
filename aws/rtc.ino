void rtcBegin() {
  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    lcd.home();
    lcd.backlight();
    lcd.print("DS3231 not found");
    while (true);
  }
  else {
    Serial.println("RTC init complete");
  }
}

void setTimeAuto() {
  #ifdef RESET_TIME
    rtc.setTime(BUILD_SEC + SEC_ADD, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  #endif
}