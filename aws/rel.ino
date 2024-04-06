void relayTick() {
  DateTime now = rtc.getTime();
  for (byte i = 0; i < 1; i++) {
    switch (ChannelMode[i]) {
    case 0:
      break;
    case 1:
      if (millis() - ChannelMillis[i] > ChannelInterval[i]) {
        ChannelMillis[i] = millis();
        relayOn(false, true, i);
      }
      break;
    case 2:
      if (now.day == ChannelsD[i] && now.hour == ChannelsH[i] && now.minute == ChannelsM[i]) {
        relayOn(false, true, i);
      }
      break;
    case 3:
      if (now.hour == ChannelsH[i] && now.minute == ChannelsM[i]) {
        relayOn(false, true, i);
      }
      break;
    } 
  }
}

void relayOn(const bool force, const bool tf, const byte channel) {
  if (!force && !gercTick()) return;

  switch (channel) {
  case 0: 
    if (tf) digitalWrite(relay1_pin, RELAY_HIGH);
    else digitalWrite(relay1_pin, RELAY_LOW);
    break;
  case 1:
    if (tf) digitalWrite(relay2_pin, RELAY_HIGH);
    else digitalWrite(relay2_pin, RELAY_LOW);
    break;
  }
}

bool gercTick() {
  return bool(digitalRead(water_sensor_pin));
}