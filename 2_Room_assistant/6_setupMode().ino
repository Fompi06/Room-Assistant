byte setupMode(bool reset_mode) {
  static byte step;
  if (reset_mode) step == 0;
  byte customArrow[] = { B00000, B00100, B00010, B11101, B11101, B00010, B00100, B00000 };
  static byte arrow = constrain(arrow, 0, 5);
  enc.tick();
  if (enc.held()) step++;
  if (step == 0) {
    enc.tick();
    byte Whours;
    byte Wmins1;
    byte Wmins2;
    if (enc.right()) Timers.Wtime++;
    if (enc.left()) Timers.Wtime--;
    if (enc.rightH()) Timers.Wtime = Timers.Wtime + 10;
    if (enc.leftH()) Timers.Wtime = Timers.Wtime - 10;
    if (Timers.Wtime > 599) Timers.Wtime = 599;
    if (Timers.Wtime < 0) {
      lcd.setCursor(0, 1);
      Timers.Wtime = -1;
      lcd.print("Default Settings");
      return 0;
    }
    Wmins2 = Timers.Wtime % 10;
    Whours = Timers.Wtime / 60;
    Wmins1 = Timers.Wtime / 10 - Whours * 6;
    lcd.setCursor(0, 1);
    lcd.print(String(Whours));
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(String(Wmins1));
    lcd.print(String(Wmins2));
    lcd.print(" mins ");
    return 0;
  }
  if (step == 1) {
    enc.tick();
    byte Rhours;
    byte Rmins1;
    byte Rmins2;
    if (enc.right()) Timers.Rtime++;
    if (enc.left()) Timers.Rtime--;
    if (enc.rightH()) Timers.Rtime = Timers.Rtime + 10;
    if (enc.leftH()) Timers.Rtime = Timers.Rtime - 10;
    if (Timers.Rtime > 599) Timers.Rtime = 599;
    if (Timers.Rtime < 0) {
      lcd.setCursor(0, 1);
      Timers.Rtime = -1;
      lcd.print("Default Settings");
      return 0;
    }
    Rmins2 = Timers.Rtime % 10;
    Rhours = Timers.Rtime / 60;
    Rmins1 = Timers.Rtime / 10 - Rhours * 6;

    lcd.setCursor(0, 1);
    lcd.print(String(Rhours));
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(String(Rmins1));
    lcd.print(String(Rmins2));
    lcd.print(" mins ");
    lcd.setCursor(0, 0);
    lcd.print("Set relax time:");
    return 0;
  }
  if (step == 2) {
    enc.tick();
    byte Fhours;
    byte Fmins1;
    byte Fmins2;
    if (enc.right()) Timers.Ftime++;
    if (enc.left()) Timers.Ftime--;
    if (enc.rightH()) Timers.Ftime = Timers.Ftime + 10;
    if (enc.leftH()) Timers.Ftime = Timers.Ftime - 10;
    if (Timers.Ftime > 599) Timers.Ftime = 599;
    if (Timers.Ftime < 0) {
      lcd.setCursor(0, 1);
      lcd.print("Default Settings");
      Timers.Ftime = -1;
      return 0;
    }
    Fmins2 = Timers.Ftime % 10;
    Fhours = Timers.Ftime / 60;
    Fmins1 = Timers.Ftime / 10 - Fhours * 6;

    lcd.setCursor(0, 1);
    lcd.print(String(Fhours));
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(String(Fmins1));
    lcd.print(String(Fmins2));
    lcd.print(" mins ");
    lcd.setCursor(0, 0);
    lcd.print("Set offline time");
    return 0;
  }
  if (step == 3) {
    enc.tick();
    if (enc.right()) arrow++;
    if (enc.left()) arrow--;
    if (enc.click()) {
      switch (arrow) {
        case 0:
          settings.strict = !settings.strict;
          ee_request = true;
          ee_reset = true;
          break;
        case 1:
          settings.checkups = !settings.checkups;
          ee_request = true;
          ee_reset = true;
          break;
        case 2:
          settings.energySave++;
          settings.energySave = settings.energySave % 3;
          ee_request = true;
          ee_reset = true;
          break;
        case 3:
          settings.stats = !settings.stats;
          ee_request = true;
          ee_reset = true;
          break;
        case 4:
          if (ee_request && millis() - TimerEE >= 5000) {
            Tupdate(TimerEE);
            ee_request = false;
            EEPROM.put(1, settings);
            lcd.setCursor(14, 0);
            lcd.print("OK");
            delay(1000);
          }
          break;
        case 5:
          if (ee_reset && millis() - TimerEEreset >= 5000) {
            Tupdate(TimerEEreset);
            ee_reset = false;
            lcd.setCursor(14, 1);
            lcd.print("OK");
            EEPROM.put(0, 'p');
            delay(1000);
          }
          break;
      }
    }
    lcd.createChar(7, customArrow);
    switch (arrow) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("Strict mode:");
        if (settings.strict) lcd.print("Да ");
        else
          lcd.print("Нет");
        lcd.setCursor(0, 1);
        lcd.print(" Enab. checks:");
        if (settings.checkups) lcd.print("Да");
        else
          lcd.print("No");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print(" Strict mode:");
        if (settings.strict) lcd.print("Да ");
        else
          lcd.print("Нет");
        lcd.setCursor(0, 1);
        lcd.write(7);
        lcd.print("Enab. checks:");
        if (settings.checkups) lcd.print("Да");
        else
          lcd.print("No");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("EnergySave:");
        if (settings.energySave == 0) lcd.print("ОТКЛ");
        else if (settings.energySave == 1)
          lcd.print(" МИН");
        else
          lcd.print(" ЭКО");
        lcd.setCursor(0, 1);
        lcd.print(" Stats:");
        if (settings.stats == 0) lcd.print("     ОТКЛ");
        else
          lcd.print("      ВКЛ");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print(" EnergySave:");
        if (settings.energySave == 0) lcd.print("ОТКЛ");
        else if (settings.energySave == 1)
          lcd.print(" МИН");
        else
          lcd.print(" ЭКО");
        lcd.setCursor(0, 1);
        lcd.write(7);
        lcd.print("Stats:");
        if (settings.stats == 0) lcd.print("     ОТКЛ");
        else
          lcd.print("      ВКЛ");
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("SaveSettings   ");
        lcd.setCursor(0, 1);
        lcd.print(" ResetSettngs   ");
        break;
      case 5:
        lcd.setCursor(0, 0);
        lcd.print(" SaveSettings   ");
        lcd.setCursor(0, 1);
        lcd.write(7);
        lcd.print("ResetSettngs   ");
        break;
    }
    return 0;
  }
  if (settings.energySave >= 1) power.hardwareDisable(PWR_SPI);
  if (settings.energySave == 2)
    return 11;

  return 1;
}