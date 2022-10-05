#define DisablePins() \
  detachInterrupt(1); \
  detachInterrupt(0)

#define EnablePins() \
  attachInterrupt(1, btnTick, FALLING); \
  attachInterrupt(0, btnTick, FALLING)

#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_F5 698
#define NOTE_A5 880
#define NOTE_B5 988
#define NOTE_D6 1175

void setup() {
#ifdef DEBUG_ENABLE
  Serial.begin(9600);
#endif
  power.hardwareEnable(PWR_ALL);
  power.calibrate(8000);
  power.correctMillis(true);
  // power.autoCalibrate();
  newTimer(Timer1);
  pinMode(SEN_VCC, OUTPUT);
  pinMode(BUZ_VCC, OUTPUT);
  digitalWrite(SEN_VCC, 1);
  digitalWrite(BUZ_VCC, 1);
  char hello2[] = { 'H', 'e', 'l', 'l', 'o', '!' };
  if (EEPROM.read(0) != 254) {
    EEPROM.put(1, settings);
    EEPROM.put(3, WTime);
    EEPROM.put(11, RTime);
    EEPROM.put(19, FTime);
    EEPROM.put(27, missedW);
    EEPROM.put(31, missedF);
    EEPROM.put(35, Wmore);
    EEPROM.put(43, Rmore);
    EEPROM.put(51, Fmore);
    EEPROM.put(59, drink);
    EEPROM.put(66, EEPcounter);
    EEPROM.put(67, EEPcounterDrink);
    EEPROM.update(0, 254);
  } else {
    EEPROM.get(1, settings);
    EEPROM.get(3, WTime);
    EEPROM.get(11, RTime);
    EEPROM.get(19, FTime);
    EEPROM.get(27, missedW);
    EEPROM.get(31, missedF);
    EEPROM.get(35, Wmore);
    EEPROM.get(43, Rmore);
    EEPROM.get(51, Fmore);
    EEPROM.get(59, drink);
    EEPROM.get(66, EEPcounter);
    EEPROM.get(67, EEPcounterDrink);
  }
  randomSeed(analogRead(0));
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(RED, OUTPUT);

  byte chars[][8] = {
    { 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b00000 },  // П (0)
    { 0b00000, 0b00000, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // и (1)
    { 0b00000, 0b00000, 0b11110, 0b10001, 0b11110, 0b10001, 0b11110, 0b00000 },  // в (2)
    { 0b00000, 0b00000, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000 },  // т (3)
    { 0b00000, 0b00000, 0b10001, 0b11011, 0b10101, 0b10001, 0b10001, 0b00000 },  // м (4)
    { 0b00000, 0b00000, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b00000 },  // н (5)
    { 0b00000, 0b00000, 0b10001, 0b10001, 0b11101, 0b10011, 0b11101, 0b00000 },  // ы (6)
    { 0b01010, 0b00100, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // й (7)
    { 0b00000, 0b00000, 0b10101, 0b10101, 0b10101, 0b10101, 0b11111, 0b00001 },  // щ ()
    { 0b00000, 0b00000, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b00000 },  // к ()
  };
  for (byte i = 0; i < 8; i++) lcd.createChar(i, chars[i]);
  uint16_t notes[] = { 415, 440, 523, 698, 880 };
  if (!bitRead(settings.sett, sounds)) {
    for (byte i = 0; i < 5; i++) notes[i] = 0;
  }
  tone(BUZ, notes[2], 200);
  lcd.setCursor(4, 0);
  lcd.write(0);
  delay(100);
  lcd.print("p");
  delay(100);
  lcd.write(1);
  delay(100);
  tone(BUZ, notes[2], 200);
  lcd.write(2);
  delay(100);

  lcd.print("e");
  delay(100);
  lcd.write(3);
  delay(100);
  tone(BUZ, notes[4], 300);
  lcd.print("!");
  delay(100);
  lcd.setCursor(5, 1);
  for (int i = 0; i < 6; i++) {
    lcd.print(hello2[i]);
    delay(100);
    if (i == 2) tone(BUZ, notes[3], 300);
  }

  delay(750);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Ko");
  delay(100);
  lcd.write(4);
  delay(100);
  lcd.write(5);
  delay(100);
  lcd.print("a");
  delay(100);
  lcd.write(3);
  delay(100);
  lcd.write(5);
  delay(100);
  lcd.write(6);
  delay(100);
  lcd.write(7);
  delay(100);
  lcd.createChar(2, chars[4]);
  lcd.setCursor(4, 1);
  lcd.write(0);
  delay(100);
  lcd.print("o");
  delay(100);
  lcd.write(4);
  delay(100);
  lcd.print("o");
  delay(100);
  lcd.write(2);
  delay(100);
  lcd.write(5);
  delay(100);
  lcd.write(1);
  delay(100);
  lcd.print("k");
  delay(750);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("by Fompi");
  delay(750);
}


void loop() {
  digitalWrite(SEN_VCC, 1);
  byte step = 0;
  static bool reset_mode;
  newTimer(Timer1);
  Timer1 = millis();
  if (reset_mode) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Exit to menu"));
  }
  lcd.setCursor(0, 0);
  lcd.blink();
  while (millis() - Timer1 <= 2000) {
    enc.tick();
    redbtn.tick();
    yellowbtn.tick();
    if (enc.hold()) {
      btnsound();
      Timers.Wtime--;
      Timers.Rtime--;
      Timers.Ftime--;
      if (settings.energySave == 2) step = 11;
      else step = 1;
      encText();
    }
    if (redbtn.hold() && yellowbtn.hold()) {
      btnsound();
      Timers.Wtime = 600;
      Timers.Rtime = 600;
      Timers.Ftime = 600;
      if (settings.energySave == 2) step = 11;
      else step = 1;
      RYText();
      break;
    }
    if (redbtn.hold()) {
      btnsound();
      Timers.Wtime = OnlineTime1;
      Timers.Rtime = RelaxTime1;
      Timers.Ftime = OffTime1;
      if (settings.energySave == 2) step = 11;
      else step = 1;
      RText();
    }
    if (yellowbtn.hold()) {
      btnsound();
      Timers.Wtime = OnlineTime2;
      Timers.Rtime = RelaxTime2;
      Timers.Ftime = OffTime2;
      if (settings.energySave == 2) step = 11;
      else step = 1;
      YText();
    }
  }
  lcd.noBlink();
  setBaseText();
  digitalWrite(SEN_VCC, 0);
  lcd.noBacklight();
  lcd.noDisplay();
  power.sleepDelay(3000);
  digitalWrite(SEN_VCC, 1);
  delay(100);
  lcd.backlight();
  lcd.display();

  while (1) {
    switch (step) {
      case 0:
        step = setupMode(&reset_mode);
        break;
      case 1:
        step = noMode(0);
        break;
      case 2:
        step = noMode(1);
        break;
      case 3:
        step = noMode(2);
        break;
      case 11:
        step = workModePow(0);
        break;
      case 12:
        step = workModePow(1);
        break;
      case 13:
        step = workModePow(2);
        break;
    }
    if (step == 99) {
      reset_mode = 1;
      step = 0;
      break;
    }
  }
}

// вспомогательные функции для оптимизации оперативной памяти
void setBaseText() {
  lcd.display();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Set online time:");
  lcd.setCursor(0, 1);
  lcd.print("0");
  lcd.print(" hrs");
  lcd.print(" : ");
  lcd.print("0");
  lcd.print("0");
  lcd.print(" mins");
}

void encText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Default Settings");
  lcd.setCursor(0, 1);
  lcd.print(" W ");
  lcd.print(ReminderOnline);
  lcd.print(" R ");
  lcd.print(ReminderRelax);
  lcd.print(" O ");
  lcd.print(ReminderOff);
  lcd.noBlink();
  delay(750);
}
void RYText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Notif. disabled");
  lcd.setCursor(0, 1);
  lcd.print("W ");
  lcd.print(600);
  lcd.print(" R ");
  lcd.print(600);
  lcd.print(" O ");
  lcd.print(600);
  lcd.noBlink();
  delay(750);
}
void RText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode 1 activated");
  lcd.setCursor(0, 1);
  lcd.print(" W ");
  lcd.print(OnlineTime1);
  lcd.print(" R ");
  lcd.print(RelaxTime1);
  lcd.print(" O ");
  lcd.print(OffTime1);
  lcd.noBlink();
  delay(750);
}
void YText() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode 2 activated");
  lcd.setCursor(0, 1);
  lcd.print(" W ");
  lcd.print(OnlineTime2);
  lcd.print(" R ");
  lcd.print(RelaxTime2);
  lcd.print(" O ");
  lcd.print(OffTime2);
  lcd.noBlink();
  delay(750);
}

void btnsound() {
#if (BTNSOUNDS == 1)
  if (bitRead(settings.sett, sounds2)) tone(BUZ, NOTE_D5, 50);
#endif
}