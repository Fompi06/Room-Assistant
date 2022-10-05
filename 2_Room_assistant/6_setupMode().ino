#define writeFlag(flag, num, state) ((state) ? bitSet(flag, num) : bitClear(flag, num))

byte setupMode(bool* reset_mode) {
  static byte step;
  if (*reset_mode) {
    step = 0;
    *reset_mode = 0;
  }
  byte customArrow[] = { B00000, B00100, B00010, B11101, B11101, B00010, B00100, B00000 };
  static byte arrow = constrain(arrow, 0, 5);
  enc.tick();
  redbtn.tick();
  yellowbtn.tick();
  if (redbtn.hold() && yellowbtn.hold()) {
    btnsound();
    Timers.Wtime = 600;
    Timers.Rtime = 600;
    Timers.Ftime = 600;
    step = 4;
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
    goto Exit;
  }
  if (redbtn.hold()) {
    btnsound();
    Timers.Wtime = OnlineTime1;
    Timers.Rtime = RelaxTime1;
    Timers.Ftime = OffTime1;
    step = 4;
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
    goto Exit;
  }
  if (yellowbtn.hold()) {
    btnsound();
    Timers.Wtime = OnlineTime2;
    Timers.Rtime = RelaxTime2;
    Timers.Ftime = OffTime2;
    step = 4;
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
    goto Exit;
  }
  if (enc.held()) {
    step++;
    btnsound();
  }
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
    lcd.print(Whours);
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(Wmins1);
    lcd.print(Wmins2);
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
    lcd.print(Rhours);
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(Rmins1);
    lcd.print(Rmins2);
    lcd.print(" mins ");
    lcd.setCursor(0, 0);
    lcd.print("Set relax time: ");
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
    lcd.print(Fhours);
    lcd.print(" hrs");
    lcd.print(" : ");
    lcd.print(Fmins1);
    lcd.print(Fmins2);
    lcd.print(" mins ");
    lcd.setCursor(0, 0);
    lcd.print("Set offline time");
    return 0;
  }
  if (step == 3) {
    enc.tick();
    if (enc.right()) {
      if (arrow != 7) {
        arrow++;
        btnsound();
      }
    }

    if (enc.left()) {
      if (arrow != 0) {
        arrow--;
        btnsound();
      }
    }
    if (enc.click()) {
      btnsound();
      switch (arrow) {
        case 0:
          writeFlag(settings.sett, strict, !bitRead(settings.sett, strict));
          ee_request = true;
          break;
        case 1:
          writeFlag(settings.sett, checkups, !bitRead(settings.sett, checkups));
          ee_request = true;
          break;
        case 2:
          settings.energySave++;
          settings.energySave = settings.energySave % 3;
          ee_request = true;
          break;
        case 3:
          if (!bitRead(settings.sett, sounds) && !bitRead(settings.sett, sounds2)) bitSet(settings.sett, sounds);
          else if (bitRead(settings.sett, sounds) && !bitRead(settings.sett, sounds2)) bitSet(settings.sett, sounds2);
          else if (bitRead(settings.sett, sounds) && bitRead(settings.sett, sounds2)) {
            bitClear(settings.sett, sounds);
            bitClear(settings.sett, sounds2);
          }
          ee_request = true;
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
          lcd.setCursor(14, 1);
          lcd.print("OK");
          EEPROM.put(0, 'p');
          delay(1000);
          break;
      }
    }
    lcd.createChar(7, customArrow);
    switch (arrow) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("Strict mode:");
        if (bitRead(settings.sett, strict)) lcd.print("Yes");
        else
          lcd.print(" No");
        lcd.setCursor(0, 1);
        lcd.print(" Checks:");
        if (bitRead(settings.sett, checkups)) lcd.print("     Yes");
        else
          lcd.print("      No");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print(" Strict mode:");
        if (bitRead(settings.sett, strict)) lcd.print("Yes");
        else
          lcd.print(" No");
        lcd.setCursor(0, 1);
        lcd.write(7);
        lcd.print("Checks:");
        if (bitRead(settings.sett, checkups)) lcd.print("     Yes");
        else
          lcd.print("      No");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("EnergySave:");
        if (settings.energySave == 0) lcd.print("Disab");
        else if (settings.energySave == 1)
          lcd.print(" Min");
        else
          lcd.print(" Eco");
        lcd.setCursor(0, 1);
        lcd.print(" Sounds:");
        if (bitRead(settings.sett, sounds)) {
          if (bitRead(settings.sett, sounds2)) lcd.print("     All");
          else lcd.print("   Alarm");
        } else lcd.print("      No");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print(" EnergySave:");
        if (settings.energySave == 0) lcd.print("Disab");
        else if (settings.energySave == 1)
          lcd.print(" Min");
        else
          lcd.print(" Eco");
        lcd.setCursor(0, 1);
        lcd.write(7);
        lcd.print("Sounds:");
        if (bitRead(settings.sett, sounds)) {
          if (bitRead(settings.sett, sounds2)) lcd.print("     All");
          else lcd.print("   Alarm");
        } else lcd.print("      No");
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.write(7);
        lcd.print("SaveSettings    ");
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
Exit:
  if (settings.energySave >= 1) power.hardwareDisable(PWR_SPI | PWR_UART0 | PWR_UART2 | PWR_UART1 | PWR_UART3);
  if (settings.energySave == 2) return 11;

  return 1;
}