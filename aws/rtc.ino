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