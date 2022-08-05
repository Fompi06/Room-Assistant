void graphs(byte mode, bool* mode_check, bool* mode_time, bool* arrow, byte arrowPosition) {
  byte lines[][8] = {
    { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111 },
    { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111 },
    { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 },
    { B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111 },
    { B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111 },
    { B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111 },
    { B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111 },
  };
  for (byte i = 0; i < 7; i++) {
    lcd.createChar(i, lines[i]);
  }
  uint16_t statsS[4];
  uint16_t statsW[4];
  uint16_t statsR[4];
  uint16_t statsF[4];
  uint8_t Wmissed[4];
  uint8_t Fmissed[4];

  for (byte i = 0; i < 4; i++) {
    statsS[i] = WTime[i] + RTime[i];
    if (statsS[i] > 320) statsS[i] = 320;
    statsS[i] = map(statsS[i], 0, 320, 0, 16);
    statsS[i] = constrain(statsS[i], 0, 16);
    statsF[i] = FTime[i];
    if (statsF[i] > 320) statsF[i] = 320;
    statsF[i] = map(statsF[i], 0, 320, 0, 16);
    statsF[i] = constrain(statsF[i], 0, 16);
    statsR[i] = RTime[i];
    if (statsR[i] > 320) statsR[i] = 320;
    statsR[i] = map(statsR[i], 0, 320, 0, 16);
    statsR[i] = constrain(statsR[i], 0, 16);
    statsW[i] = WTime[i];
    if (statsW[i] > 320) statsW[i] = 320;
    statsW[i] = map(statsW[i], 0, 320, 0, 16);
    statsW[i] = constrain(statsW[i], 0, 16);
    Wmissed[i] = missedW[i];
    Fmissed[i] = missedF[i];
    if (Wmissed[i] > 16) Wmissed[i] = 16;
    if (Fmissed[i] > 16) Fmissed[i] = 16;
  }

  Timers.STimeEE = Timers.RTimeEE + Timers.WTimeEE;
  StatsNowS = Timers.STimeEE;
  if (StatsNowS > 320) StatsNowS = 320;
  StatsNowS = map(StatsNowS, 0, 320, 0, 16);
  StatsNowS = constrain(StatsNowS, 0, 16);
  StatsNowW = Timers.WTimeEE;
  if (StatsNowW > 320) StatsNowW = 320;
  StatsNowW = map(StatsNowW, 0, 320, 0, 16);
  StatsNowW = constrain(StatsNowW, 0, 16);
  StatsNowR = Timers.RTimeEE;
  if (StatsNowR > 320) StatsNowR = 320;
  StatsNowR = map(StatsNowR, 0, 320, 0, 16);
  StatsNowR = constrain(StatsNowR, 0, 16);
  StatsNowF = Timers.FTimeEE;
  if (StatsNowF > 320) StatsNowF = 320;
  StatsNowF = map(StatsNowF, 0, 320, 0, 16);
  StatsNowF = constrain(StatsNowF, 0, 16);
  if (missedNowW > 16) missedNowW = 16;
  if (missedNowF > 16) missedNowF = 16;


  if (*mode_time) {
    if (*mode_check) {
      lcd.clear();
      *mode_check = 0;
    }
    enc.tick();
    if (enc.right()) *arrow = !*arrow;
    if (enc.left()) *arrow = !*arrow;

    lcd.setCursor(0, 0);
    if (!*arrow) {
      lcd.write(126);
      switch (mode) {
        case 0:
          lcd.print("Online:");
          if (arrowPosition != 4) lcd.print(String(WTime[arrowPosition] + RTime[arrowPosition]));
          else
            lcd.print(String(StatsNowS));
          break;
        case 1:
          lcd.print("Work:");
          if (arrowPosition != 4) lcd.print(String(WTime[arrowPosition]));
          else
            lcd.print(String(StatsNowW));
          break;
        case 2:
          lcd.print("Work missed:");
          if (arrowPosition != 4) lcd.print(String(missedW[arrowPosition]));
          else
            lcd.print(String(missedNowW));
          break;
      }
      if (mode != 3) lcd.print(" mins");
      lcd.setCursor(0, 1);
      switch (mode) {
        case 0:
          lcd.print(" Offline:");
          if (arrowPosition != 4) lcd.print(String(FTime[arrowPosition]));
          else
            lcd.print(String(StatsNowF));
          break;
        case 1:
          lcd.print(" Relax:");
          if (arrowPosition != 4) lcd.print(String(RTime[arrowPosition]));
          else
            lcd.print(String(StatsNowR));
          break;
        case 2:
          lcd.print(" Offline missed:");
          if (arrowPosition != 4) lcd.print(String(missedF[arrowPosition]));
          else
            lcd.print(String(missedNowF));
          break;
      }

      if (mode != 2) lcd.print(" mins");
    } else {

      switch (mode) {
        case 0:
          lcd.print(" Online:");
          if (arrowPosition != 4) lcd.print(String(WTime[arrowPosition] + RTime[arrowPosition]));
          else
            lcd.print(String(StatsNowS));
          break;
        case 1:
          lcd.print(" Work:");
          if (arrowPosition != 4) lcd.print(String(WTime[arrowPosition]));
          else
            lcd.print(String(StatsNowW));
          break;
        case 2:
          lcd.print(" Work missed:");
          if (arrowPosition != 4) lcd.print(String(missedW[arrowPosition]));
          else
            lcd.print(String(missedNowW));
          break;
      }
      if (mode != 2) lcd.print(" mins");
      lcd.setCursor(0, 1);
      lcd.write(126);
      switch (mode) {
        case 0:
          lcd.print("Offline:");
          if (arrowPosition != 4) lcd.print(String(FTime[arrowPosition]));
          else
            lcd.print(String(StatsNowF));
          break;
        case 1:
          lcd.print("Relax:");
          if (arrowPosition != 4) lcd.print(String(RTime[arrowPosition]));
          else
            lcd.print(String(StatsNowR));
          break;
        case 2:
          lcd.print("Offline missed:");
          if (arrowPosition != 4) lcd.print(String(missedF[arrowPosition]));
          else
            lcd.print(String(missedNowF));
          break;
      }
      if (mode != 2) lcd.print(" mins");
    }
    return;
  }

  if (*mode_check) {
    lcd.clear();
    *mode_check = 0;
  }
  if (arrowPosition == 0) {
    lcd.setCursor(0, 1);
    lcd.write(126);
    lcd.setCursor(3, 0);
    lcd.write(127);
    lcd.setCursor(3, 1);
    lcd.write(127);
  } else {
    lcd.setCursor(arrowPosition * 3, 0);
    lcd.write(126);
    lcd.setCursor(arrowPosition * 3, 1);
    lcd.write(126);
    lcd.setCursor(arrowPosition * 3 + 3, 0);
    lcd.write(127);
    lcd.setCursor(arrowPosition * 3 + 3, 1);
    lcd.write(127);
  }
  int8_t m = EEPcounter;
  m--;
  if (mode == 0) {
    byte PCChar[] = { 0x00, 0x1F, 0x11, 0x11, 0x1F, 0x04, 0x0E, 0x00 };
    lcd.createChar(7, PCChar);
    lcd.setCursor(0, 0);
    lcd.write(7);
    for (byte i = 0; i < 4; i++) {

      m++;
      m = m % 4;

      lcd.setCursor(i * 3 + 1, 1);
      if (statsS[m] == 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 1, 0);
        lcd.write(255);
      } else if (statsS[m] > 8 && statsS[m] < 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 1, 0);
        lcd.write(statsS[m] - 9);
      } else if (statsS[m] > 0 && statsS[m] <= 8)
        lcd.write(statsS[m] - 1);

      lcd.setCursor(i * 3 + 2, 1);
      if (statsF[m] == 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 2, 0);
        lcd.write(255);
      } else if (statsF[m] > 8 && statsF[m] < 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 2, 0);
        lcd.write(statsF[m] - 9);
      } else if (statsF[m] > 0 && statsF[m] <= 8)
        lcd.write(statsF[m] - 1);
    }

    lcd.setCursor(13, 1);
    if (StatsNowS == 16) {
      lcd.write(255);
      lcd.setCursor(13, 0);
      lcd.write(255);
    } else if (StatsNowS > 8 && StatsNowS < 16) {
      lcd.write(255);
      lcd.setCursor(13, 0);
      lcd.write(StatsNowS - 9);
    } else if (StatsNowS > 0 && StatsNowS <= 8)
      lcd.write(StatsNowS - 1);

    lcd.setCursor(14, 1);
    if (StatsNowF == 16) {
      lcd.write(255);
      lcd.setCursor(14, 0);
      lcd.write(255);
    } else if (StatsNowF > 8 && StatsNowF < 16) {
      lcd.write(255);
      lcd.setCursor(14, 0);
      lcd.write(StatsNowF - 9);
    } else if (StatsNowF > 0 && StatsNowF <= 8)
      lcd.write(StatsNowF - 1);
    return;
  }

  if (mode == 1) {
    byte HumanChar[] = { 0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A };
    lcd.createChar(7, HumanChar);
    lcd.setCursor(0, 0);
    lcd.write(7);
    for (byte i = 0; i < 4; i++) {

      m++;
      m = m % 4;

      lcd.setCursor(i * 3 + 1, 1);
      if (statsR[m] == 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 1, 0);
        lcd.write(255);
      } else if (statsR[m] > 8 && statsR[m] < 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 1, 0);
        lcd.write(statsR[m] - 9);
      } else if (statsR[m] > 0 && statsR[m] <= 8)
        lcd.write(statsR[m] - 1);
      lcd.setCursor(i * 3 + 2, 1);
      if (statsW[m] == 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 2, 0);
        lcd.write(255);
      } else if (statsW[m] > 8 && statsW[m] < 16) {
        lcd.write(255);
        lcd.setCursor(i * 3 + 2, 0);
        lcd.write(statsW[m] - 9);
      } else if (statsW[m] > 0 && statsW[m] <= 8)
        lcd.write(statsW[m] - 1);
    }

    lcd.setCursor(13, 1);
    if (StatsNowR == 16) {
      lcd.write(255);
      lcd.setCursor(13, 0);
      lcd.write(255);
    } else if (StatsNowR > 8 && StatsNowR < 16) {
      lcd.write(255);
      lcd.setCursor(13, 0);
      lcd.write(StatsNowR - 9);
    } else if (StatsNowR > 0 && StatsNowR <= 8)
      lcd.write(StatsNowR - 1);
    lcd.setCursor(14, 1);
    if (StatsNowW == 16) {
      lcd.write(255);
      lcd.setCursor(14, 0);
      lcd.write(255);
    } else if (StatsNowW > 8 && StatsNowW < 16) {
      lcd.write(255);
      lcd.setCursor(14, 0);
      lcd.write(StatsNowW - 9);
    } else if (StatsNowW > 0 && StatsNowW <= 8)
      lcd.write(StatsNowW - 1);
    return;
  }

  byte CharButton[8] = { B01110, B10001, B10101, B10101, B10001, B10101, B10001, B01110 };
  lcd.createChar(7, CharButton);
  lcd.setCursor(0, 0);
  lcd.write(7);
  for (byte i = 0; i < 4; i++) {

    m++;
    m = m % 4;

    lcd.setCursor(i * 3 + 1, 1);
    if (Wmissed[m] == 16) {
      lcd.write(255);
      lcd.setCursor(i * 3 + 1, 0);
      lcd.write(255);
    } else if (Wmissed[m] > 8 && Wmissed[m] < 16) {
      lcd.write(255);
      lcd.setCursor(i * 3 + 1, 0);
      lcd.write(Wmissed[m] - 9);
    } else if (Wmissed[m] > 0 && Wmissed[m] <= 8)
      lcd.write(Wmissed[m] - 1);
    lcd.setCursor(i * 3 + 2, 1);
    if (Fmissed[m] == 16) {
      lcd.write(255);
      lcd.setCursor(i * 3 + 2, 0);
      lcd.write(255);
    } else if (Fmissed[m] > 8 && Fmissed[m] < 16) {
      lcd.write(255);
      lcd.setCursor(i * 3 + 2, 0);
      lcd.write(Fmissed[m] - 9);
    } else if (Fmissed[m] > 0 && Fmissed[m] <= 8)
      lcd.write(Fmissed[m] - 1);
  }

  lcd.setCursor(13, 1);
  if (missedNowW == 16) {
    lcd.write(255);
    lcd.setCursor(13, 0);
    lcd.write(255);
  } else if (missedNowW > 8 && missedNowW < 16) {
    lcd.write(255);
    lcd.setCursor(13, 0);
    lcd.write(missedNowW - 9);
  } else if (missedNowW > 0 && missedNowW <= 8)
    lcd.write(missedNowW - 1);

  lcd.setCursor(14, 1);
  if (missedNowF == 16) {
    lcd.write(255);
    lcd.setCursor(14, 0);
    lcd.write(255);
  } else if (missedNowF > 8 && missedNowF < 16) {
    lcd.write(255);
    lcd.setCursor(14, 0);
    lcd.write(missedNowF - 9);
  } else if (missedNowF > 0 && missedNowF <= 8)
    lcd.write(missedNowF - 1);
  // сделать только для пропущенных кнопок
}