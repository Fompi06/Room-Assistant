byte noMode(byte globMode) {
  // return 1 = workMode();
  // return 2 = relaxMode();
  // return 3 = offlineMode();
  lcd.clear();
  newTimer(Timer1);
  newTimer(Timer2);
  uint16_t randomTime = random(10, 30);
  byte randomAlert = random(0, randomAlertTime);
  if (bitRead(settings.sett, strict)) randomAlert = 0;
  if (!bitRead(settings.sett, checkups) || globMode != 0) randomTime = 3000;
  digitalWrite(RED, 0);
  uint16_t RepeatedTimer = 10;
  uint8_t halfTimer = 5;
  uint16_t WorkTimer;
  uint8_t PerNow1 = 0;
  uint16_t RtimeL0 = 0;
  uint16_t RtimeL = 0;
  uint16_t FtimeL0 = 0;
  uint16_t FtimeL = 0;
  switch (globMode) {
    case 0:
      if (Timers.Wtime < 0) {
        Timers.Wtime = ReminderOnline;
        RepeatedTimer = RepeatedReminderOnline;
      } else if (Timers.Wtime > 50) RepeatedTimer = Timers.Wtime * 0.2;
      else if (Timers.Wtime == 0) Timers.Wtime = 600;
      WorkTimer = Timers.Wtime + randomAlert;
      PerNow1 = Timers.Wtime / 100;
      if (bitRead(settings.sett, sounds)) halfTimer = RepeatedTimer >> 1;
      break;
    case 1:
      if (Timers.Rtime < 0) {
        Timers.Rtime = ReminderRelax;
        RepeatedTimer = RepeatedReminderRelax;
      } else if (Timers.Rtime > 50) RepeatedTimer = Timers.Rtime * 0.2;
      else if (Timers.Rtime == 0) Timers.Rtime = 600;
      WorkTimer = Timers.Rtime + randomAlert;
      PerNow1 = Timers.Rtime / 100;
      if (bitRead(settings.sett, sounds)) halfTimer = RepeatedTimer >> 1;
      break;
    case 2:
      Timers.STimeEE = 0;
      Timers.STimeEERep = 0;
      if (Timers.Ftime < 0) {
        Timers.Ftime = ReminderOff;
        RepeatedTimer = RepeatedReminderOff;
      } else if (Timers.Ftime > 50) RepeatedTimer = Timers.Ftime * 0.2;
      else if (Timers.Ftime == 0) Timers.Ftime = 600;
      WorkTimer = Timers.Ftime + randomAlert;
      PerNow1 = Timers.Ftime / 100;
      if (bitRead(settings.sett, sounds)) halfTimer = RepeatedTimer >> 1;
      break;
  }
  static byte hours[3], mins[3], secs[3];
  byte arrowPosition = 0;
  bool missed = false;  // добавляем переменную, которая будет хранить: true - если человек пропустил отдых, false - если человек не пропускал отдых
  byte secCounter = 0;
  bool flag = 1;
  int8_t mode = 0;
  uint16_t timeL = 0;
  bool mode_check = 1;
  bool mode_time = 0;
  bool arrow = 0;
  Tupdate(Timer1);
  redbtn.resetState();
  yellowbtn.resetState();
  delay(100);
  noTone(BUZ);
  while (1) {
    // Serial.println(freeMemory());
    byte customChar[8] = { B00000, B00000, B01110, B10101, B10101, B10001, B01110, B00000 };
    switch (globMode) {
      case 0:
        switch (secCounter % 4) {
          case 0:
            customChar[4] = B10101;
            customChar[5] = B10001;
            break;
          case 1:
            customChar[4] = B10111;
            customChar[5] = B10001;
            break;
          case 2:
            customChar[4] = B10101;
            customChar[5] = B10101;
            break;
          case 3:
            customChar[4] = B11101;
            customChar[5] = B10001;
            break;
        }
        break;
      case 1:
        switch ((secCounter % 8) / 2) {
          case 0:
            customChar[4] = B10101;
            customChar[5] = B10001;
            break;
          case 1:
            customChar[4] = B10111;
            customChar[5] = B10001;
            break;
          case 2:
            customChar[4] = B10101;
            customChar[5] = B10101;
            break;
          case 3:
            customChar[4] = B11101;
            customChar[5] = B10001;
            break;
        }
        break;
      case 2:
        switch (secCounter % 2) {
          case 0:
            customChar[3] = B10001;
            customChar[4] = B10101;
            break;
          case 1:
            customChar[3] = B10001;
            customChar[4] = B10001;
            break;
        }
        break;
    }


    switch (mode) {
      case 0:
        mode_check = 1;
        switches(globMode, 0, hours[globMode], mins[globMode], secs[globMode], customChar);
        break;
      case 1:
        mode_check = 1;
        switches(globMode, 1, hours[globMode], mins[globMode], secs[globMode], customChar);
        break;
      case 2:
        graphs(0, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
      case 3:
        graphs(1, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
      case 4:
        graphs(2, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
      case 5:
        graphs(3, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
      case 6:
        graphs(4, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
    }
    if (!missed) digitalWrite(RED, 0);
    bool flag1 = 1;

    wait(Timer1, 1000) {
      yellowbtn.tick();
      redbtn.tick();
      switch (globMode) {
        case 0:
          if (redbtn.click()) return 3;
          if (yellowbtn.click()) return 2;
          break;
        case 1:
          if (redbtn.click()) return 3;
          if (yellowbtn.click()) return 1;
          break;
        case 2:
          if (redbtn.click()) return 1;
          if (yellowbtn.click()) return 2;
          break;
      }
      if (redbtn.held() && yellowbtn.held()) return 99;
      if (missed && (millis() - Timer2 >= 1000)) {
        digitalWrite(RED, flag = !flag);
        Tupdate(Timer2);
      }
      if (enc.tick()) {
        if (!mode_time && enc.right()) {
          btnsound();
          mode++;
          mode = mode % 7;
          mode_check = 1;
        }
        if (!mode_time && enc.left()) {
          btnsound();
          if (mode == 0) mode = 6;
          else
            mode--;
          mode_check = 1;
        }
        if (mode == 1 && enc.click()) {
          btnsound();
          numberDrinks++;
          if (numberDrinks == 10 && bitRead(settings.sett, sounds)) {
            tone(BUZ, NOTE_B5, 150);
            delay(200);
            tone(BUZ, NOTE_B5, 150);
            delay(200);
            tone(BUZ, NOTE_D6, 700);
            delay(700);
          }
        }
        if (mode == 1 && enc.held()) {
          btnsound();
          numberDrinks = 0;
        }
        if (mode >= 2 && enc.held()) {
          WTime[EEPcounter] = Timers.WTimeEE;
          RTime[EEPcounter] = Timers.RTimeEE;
          FTime[EEPcounter] = Timers.FTimeEE;
          missedW[EEPcounter] = missedNowW;
          missedF[EEPcounter] = missedNowF;
          Wmore[EEPcounter] = WmoreNow;
          Rmore[EEPcounter] = RmoreNow;
          Fmore[EEPcounter] = FmoreNow;
          drink[EEPcounterDrink] = numberDrinks;
          EEPcounter++;
          EEPcounter = EEPcounter % 4;
          EEPcounterDrink++;
          EEPcounterDrink = EEPcounterDrink % 7;
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
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("Data saved!");
          if (bitRead(settings.sett, sounds)) {
            tone(BUZ, NOTE_A4, 200);
            delay(300);
            tone(BUZ, NOTE_A4, 200);
            delay(200);
            delay(500);
          } else delay(1000);
        }
        if (!mode_time && mode >= 2 && enc.rightH()) {
          arrowPosition++;
          arrowPosition = arrowPosition % 5;
          mode_check = 1;
        }
        if (!mode_time && mode >= 2 && enc.leftH()) {
          if (arrowPosition == 0) arrowPosition = 4;
          else
            arrowPosition--;
          mode_check = 1;
        }
        if (!mode_time && mode >= 2 && enc.click()) {
          btnsound();
          mode_time = 1;
          mode_check = 1;
        } else if (mode_time && enc.click()) {
          btnsound();
          mode_time = 0;
          mode_check = 1;
        }
        if (mode_time && mode == 2 && enc.leftH()) {
          btnsound();
          if (arrowPosition != 4) FTime[arrowPosition] = FTime[arrowPosition] - 10;
          else
            Timers.FTimeEE = Timers.FTimeEE - 10;
        }
        if (mode_time && mode == 2 && enc.rightH()) {
          btnsound();
          if (arrowPosition != 4) FTime[arrowPosition] = FTime[arrowPosition] + 10;
          else
            Timers.FTimeEE = Timers.FTimeEE + 10;
        }
        if (mode_time && mode == 3 && enc.leftH()) {
          btnsound();
          if (arrowPosition != 4) {
            if (!arrow) WTime[arrowPosition] = WTime[arrowPosition] - 10;
            else
              RTime[arrowPosition] = RTime[arrowPosition] - 10;
          } else {
            if (!arrow) Timers.WTimeEE = Timers.WTimeEE - 10;
            else
              Timers.RTimeEE = Timers.RTimeEE - 10;
          }
        }
        if (mode_time && mode == 3 && enc.rightH()) {
          btnsound();
          if (arrowPosition != 4) {
            if (!arrow) WTime[arrowPosition] = WTime[arrowPosition] + 10;
            else
              RTime[arrowPosition] = RTime[arrowPosition] + 10;
          } else {
            if (!arrow) Timers.WTimeEE = Timers.WTimeEE + 10;
            else
              Timers.RTimeEE = Timers.RTimeEE + 10;
          }
        }
        if (mode_time && mode == 6 && enc.leftH()) {
          btnsound();
          if (arrowPosition != 7) {
            drink[arrowPosition] = drink[arrowPosition] - 1;
          } else {
            numberDrinks = numberDrinks - 1;
          }
        }
        if (mode_time && mode == 6 && enc.rightH()) {
          btnsound();
          if (arrowPosition != 7) {
            drink[arrowPosition] = drink[arrowPosition] + 1;
          } else {
            numberDrinks = numberDrinks + 1;
          }
        }
        enc.resetState();
      }
    }
    Tupdate(Timer1);
    secs[globMode]++;
    secCounter++;
    if (secs[globMode] >= 60) {
      secs[globMode] = secs[globMode] % 60;
      mins[globMode]++;
      timeL++;
      switch (globMode) {
        case 0:
          Timers.WTimeEE++;
          Timers.STimeEE++;
          break;
        case 1:
          Timers.RTimeEE++;
          Timers.STimeEE++;
          RtimeL++;
          break;
        case 2:
          Timers.FTimeEE++;
          FtimeL++;
          break;
      }
      if (missed) Timers.STimeEERep++;
      if (missed) WmoreNow++;
      if (bitRead(settings.sett, sounds) && (globMode != 2 && ((missed && Timers.STimeEERep == halfTimer))) || (globMode == 2 && ((missed && FtimeL0 == halfTimer))) || (globMode == 1 && ((missed && RtimeL0 == halfTimer)))) tone(BUZ, NOTE_GS4, 200);
    }
    if (mins[globMode] >= 60) {
      mins[globMode] = mins[globMode] % 60;
      hours[globMode]++;
    }
    if (hours[globMode] >= 10) hours[globMode] = 0;

    if ((globMode != 2 && ((missed && Timers.STimeEERep >= RepeatedTimer) || (!missed && Timers.STimeEE >= WorkTimer))) || (globMode == 2 && ((missed && FtimeL0 >= RepeatedTimer) || (!missed && FtimeL >= WorkTimer))) || (globMode == 1 && ((missed && RtimeL0 >= RepeatedTimer) || (!missed && RtimeL >= WorkTimer)))) {
      Timers.STimeEERep = 0;
      Timers.STimeEE = 0;
      byte customCharAlert[] = { B01110, B10001, B10101, B10101, B10001, B10101, B10001, B01110 };
      byte modeMass = 0;
      if (globMode == 2 && ((missed && FtimeL0 >= RepeatedTimer) || (!missed && FtimeL >= WorkTimer))) modeMass = 2;
      switches(0, modeMass + 10, hours[globMode], mins[globMode], secs[globMode], customCharAlert);
      digitalWrite(RED, 1);

      for (int i = 0; i <= 6; i++)  // 10 раз повторяем манипуляции с зуммером
      {
        Timer2 = millis();
        while (millis() - Timer2 < 1000)  // ждем одну секунду, постоянно проверяя, нажата ли кнопка
        {
          yellowbtn.tick();
          redbtn.tick();
          switch (globMode) {
            case 0:
              if (redbtn.click()) return 3;
              if (yellowbtn.click()) return 2;
              break;
            case 1:
              if (redbtn.click()) return 3;
              if (yellowbtn.click()) return 1;
              break;
            case 2:
              if (redbtn.click()) return 1;
              if (yellowbtn.click()) return 2;
              break;
          }
        }
        secs[globMode]++;
        flag1 = !flag1;
        if (secs[globMode] >= 60) {
          secs[globMode] = secs[globMode] % 60;
          mins[globMode]++;
          timeL++;
          switch (globMode) {
            case 0:
              Timers.WTimeEE++;
              Timers.STimeEE++;
              break;
            case 1:
              Timers.RTimeEE++;
              Timers.STimeEE++;
              break;
            case 2:
              Timers.FTimeEE++;
              break;
          }
        }
        if (mins[globMode] >= 60) {
          mins[globMode] = mins[globMode] % 60;
          hours[globMode]++;
        }
        if (hours[globMode] >= 10) {
          hours[globMode] = 0;
        }
        switches(globMode, 10 + globMode, hours[globMode], mins[globMode], secs[globMode], customCharAlert);
        switch (flag1) {
          case 0:
            tone(BUZ, 900);
            break;
          case 1:
            noTone(BUZ);
            break;
        }
      }
      noTone(BUZ);
      missed = 1;
      missedNowW++;
      FtimeL0 = FtimeL;
      RtimeL0 = RtimeL;
    }

    if (Timers.WTimeEE + Timers.RTimeEE + Timers.FTimeEE >= reminderDrinkVariable) {
      for (byte i = 0; i < 2; i++) {
        digitalWrite(RED, 1);
        tone(BUZ, 600);
        Tupdate(Timer1);
        delay(250);
        noTone(BUZ);
        digitalWrite(RED, 0);
        delay(250);
      }
      secs[globMode]++;
      lcd.setCursor(0, 1);
      lcd.print("Drink water!    ");
      delay(1000);
      secs[globMode]++;
      delay(1000);
      secs[globMode]++;
      reminderDrinkVariable = reminderDrinkVariable + Timers.WTimeEE + Timers.RTimeEE + +Timers.FTimeEE;
    }
    if (timeL == randomTime)  // если наш счетчик совпал со значением переменной, начинаем "опрашивать" человека
    {
      bool checkup = 0;              // создаем переменную: true - человек нажал на кнопку, false - человек не нажал на кнопку
      for (int i = 0; i <= 20; i++)  // повторяем наш цикл 20 раз, чтобы человек успел нажать на кнопку, если ему это нужно
      {
        tone(BUZ, 700);        // подаем сигнал оповещения с помощью пищалки
        digitalWrite(RED, 1);  // включаем светодиод
        Timer1 = millis();
        while (millis() - Timer1 <= 500) {  // ждем 500 мс, постоянно опрашивая кнопку
          yellowbtn.tick();
          redbtn.tick();
          if (redbtn.click()) {
            checkup = 1;  // если человек нажал на кнопку, присвоить переменной true и досрочно выйти из цикла
            break;
          }
          if (yellowbtn.click()) {
            checkup = 1;  // если человек нажал на кнопку, присвоить переменной true и досрочно выйти из цикла
            break;
          }
        }
        noTone(BUZ);
        digitalWrite(RED, 0);
        Timer1 = millis();
        while (millis() - Timer1 <= 250) {
          yellowbtn.tick();
          redbtn.tick();
          if (redbtn.click()) {
            checkup = 1;
            break;
          }
          if (yellowbtn.click()) {
            checkup = 1;  // если человек нажал на кнопку, присвоить переменной true и досрочно выйти из цикла
            break;
          }
        }
        if (checkup) {
          randomTime = timeL + random(10, 50);
          break;
        }
      }
      if (!checkup) {
        lcd.clear();  // выводим на экран о том, что кнопка не нажата, и работа стоит
        lcd.setCursor(3, 0);
        lcd.print("Idle Work");
        lcd.setCursor(0, 1);
        lcd.print("Видимо, вы ушли(");
        delay(2000);
        return 3;  // если кнопку так и никто не нажал, отправляем таймер на режим отдыха =)
      }
      digitalWrite(RED, 0);
      noTone(BUZ);
    }
  }
}