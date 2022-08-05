void setup() {
  Serial.begin(9600);
  power.hardwareEnable(PWR_ALL);
  power.autoCalibrate();
  newTimer(Timer1);
  char hello2[] = { 'h', 'e', 'l', 'l', 'o', '!' };
  String hello1[] = { "П", "р", "и", "в", "е", "т", "!", ":", ")" };
  if (EEPROM.read(0) != 254) {
    EEPROM.put(1, settings);
    EEPROM.put(5, WTime);
    EEPROM.put(13, RTime);
    EEPROM.put(21, FTime);
    EEPROM.put(29, missedW);
    EEPROM.put(33, missedF);
    EEPROM.put(37, EEPcounter);
    EEPROM.update(0, 254);
  } else {
    EEPROM.get(1, settings);
    EEPROM.get(5, WTime);
    EEPROM.get(13, RTime);
    EEPROM.get(21, FTime);
    EEPROM.get(29, missedW);
    EEPROM.get(33, missedF);
    EEPROM.get(37, EEPcounter);
  }
  randomSeed(analogRead(0));
  dht.begin();
  lcd.init();
  lcd.backlight();
  // redbtn.begin();
  // yellowbtn.begin();
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  lcd.setCursor(4, 0);
  for (int i = 0; i < 9; i++) {
    lcd.print(hello1[i]);
    delay(100);
  }
  lcd.setCursor(5, 1);
  for (int i = 0; i < 6; i++) {
    lcd.print(String(hello2[i]));
    delay(100);
  }
  // Timer1 = millis();
  // while(millis() - Timer1 <= 1000) {}
  // lcd.setCursor(3,0);
  // lcd.print("Комнатный");
  // lcd.print("Сделано Fompi");
}



void loop() {
  byte step = 0;
  bool reset_mode = 0;
  newTimer(Timer1);
  Timer1 = millis();
  while (millis() - Timer1 <= 2000) {}
  lcd.setCursor(1, 0);
  lcd.print(F("Set work time:"));
  lcd.setCursor(0, 1);
  lcd.print("0");
  lcd.print(F(" hrs"));
  lcd.print(F(" : "));
  lcd.print("0");
  lcd.print("0");
  lcd.print(F(" mins"));
  while (1) {
    switch (step) {
      case 0:
        step = setupMode(reset_mode);
        break;
      case 1:
        step = workMode();
        break;
      case 2:
        step = noMode();
        break;
      case 3:
        step = relaxMode();
        break;
      case 4:
        step = offlineMode();
        break;
      case 11:
        step = workModePow();
        break;
    }
    if (step == 99) {
      reset_mode = 1;
      break;
    }
  }
}