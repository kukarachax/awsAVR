void relayTick() {
  DateTime now = rtc.getTime();
  bool startWatering = false;
  bool stopWatering = false;

  for (uint8_t i = 0; i < 2; i++){
    if (!isGerconEnabled() && watering[i] != 0) {
      TimerTimeoutPause(i);
      relaySendSignal(RELAY_DISABLED, i);
      watering[i] = 2;
      return;
    }
  }

  for (uint8_t i = 0; i < 2; i++) {
    if (isGerconEnabled() && watering[i] == 2) startWatering = true;
    
    stopWatering = ChannelMode[i] == 0;
    stopWatering = isChannelTimerTimedOut(i);

    if (watering[i]) {
      if (!stopWatering && !startWatering) return;
    }

    if (watering[i] != 2 && ChannelMode[i] != 0) {
      for (uint8_t u = 0; u < 2; u++) {
        if (watering[u] == 1) return; 
      }
      startWatering = isDay(now.day, i) && now.hour == ChannelsH[i] && now.minute == ChannelsM[i] && !watering[i];
    }

    if (startWatering) {
      tone(buzzer_pin, 3000, 500);
      startChannelTimerOff(i);
      relaySendSignal(RELAY_ENABLED, i);
      watering[i] = 1;
    }
    if (stopWatering) {
      relaySendSignal(RELAY_DISABLED, i);
      watering[i] = 0;
    }
  }
}

bool isDay(const uint8_t day, const uint8_t i) {
  if (day == 1 && ChannelByDays1[i] == 1) return true;
  if (day == 2 && ChannelByDays2[i] == 1) return true;
  if (day == 3 && ChannelByDays3[i] == 1) return true;
  if (day == 4 && ChannelByDays4[i] == 1) return true;
  if (day == 5 && ChannelByDays5[i] == 1) return true;
  if (day == 6 && ChannelByDays6[i] == 1) return true;
  if (day == 7 && ChannelByDays7[i] == 1) return true;
  return false;
}

void relaySendSignal(const uint8_t signal, const uint8_t channel) {
  uint8_t relay_pin;

  switch (channel) {
  case 0:
    relay_pin = relay1_pin;
    break;
  case 1:
    relay_pin = relay2_pin;
    break;

  default:
    // Unexpected channel
    return;
  }

  digitalWrite(relay_pin, signal);
  watering[channel] = signal == RELAY_ENABLED;
}

bool isGerconEnabled() {
  #ifndef ENABLE_GERCON
    return true;
  #endif 

  return (bool) digitalRead(water_sensor_pin);
}

bool timerPaused[2] = {0,0}; //0 - Not Paused; 1 - Paused.

bool isChannelTimerTimedOut(const uint8_t i) {
  switch (i) {
    case 0:
      return TimerChannel1.isReady();
      break;
    case 1:
      return TimerChannel2.isReady();
      break;
  }
}

void startChannelTimerOff(const uint8_t i) {
  switch (i) {
    case 0:
      if (timerPaused[i]) TimerChannel1.resume();
      else TimerChannel1.setTimeout(ChannelTimeout[i]);
      break;
    case 1:
      if (timerPaused[i]) TimerChannel2.resume();
      else TimerChannel2.setTimeout(ChannelTimeout[i]);
      break;
  }
  
  timerPaused[i] = false;
}

void TimerTimeoutPause(const uint8_t i) {
  timerPaused[i] = true;

  switch (i) {
    case 0:
      TimerChannel1.stop();
      break;
    case 1:
      TimerChannel2.stop();
      break;
  }
} 