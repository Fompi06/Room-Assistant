byte workMode() {
  // return 3 = relaxMode();
  // return 4 = offlineMode();
  if (Timers.Wtime == 0) return 2;
  lcd.clear();
  newTimer(Timer1);
  newTimer(Timer2);
  uint16_t randomTime = random(10, 30);
  byte randomAlert = random(0, randomAlertTime);
  if (settings.strict) randomAlert = 0;
  if (!settings.checkups) randomTime = 3000;
  digitalWrite(GREEN, 0);  // отключаем все светодиоды
  digitalWrite(RED, 0);
  uint16_t WorkTimer = Timers.Wtime + randomAlert;
  uint16_t RepeatedTimer = 10;
  if (Timers.Wtime < 0) Timers.Wtime = ReminderWork;
  if (Timers.Wtime > 50) RepeatedTimer = Timers.Wtime * 0.2;
  uint8_t PerNow1 = Timers.Wtime / 100;
  static byte hours, mins, secs;
  byte arrowPosition = 0;
  bool missed = false;  // добавляем переменную, которая будет хранить: true - если человек пропустил отдых, false - если человек не пропускал отдых
  uint16_t timeL = 0;
  uint16_t timeL0 = 0;
  byte secCounter = 0;
  bool flag = 1;
  int8_t mode = 0;
  bool mode_check = 1;
  bool mode_time = 0;
  bool arrow = 0;

  Tupdate(Timer1);
  while (1) {
    noTone(BUZ);
    byte customChar[8] = { B00000, B00000, B01110, B10101, B10101, B10001, B01110, B00000 };
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

    switch (mode) {
      case 0:
        mode_check = 1;
        switches(0, 0, &hours, &mins, &secs, customChar);
        break;
      case 1:
        mode_check = 1;
        switches(0, 1, &hours, &mins, &secs, customChar);
        break;
      case 2:
        // byte customBlob[] = { B00100, B01110, B01110, B11111, B11111, B11111, B11111, B01110 };
        graphs(0, &mode_check, &mode_time, &arrow, arrowPosition);
        // сделать обычные графики, использовать условия для создания нужным нам графиков не загромождая память
        // на след странице сделать... соотношение работы и отдыха
        // на последней странице сделать кол-во пропущенных нажатий
        break;
      case 3:
        graphs(1, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
      case 4:
        graphs(2, &mode_check, &mode_time, &arrow, arrowPosition);
        break;
    }  // СДЕЛАТЬ 5 РЕЖИМ
       // сделать график "соблюдения" времени?
    if (!missed) digitalWrite(RED, 0);
    bool flag1 = 1;

    wait(Timer1, 1000) {
      yellowbtn.tick();
      redbtn.tick();
      if (yellowbtn.click()) return 3;
      if (redbtn.click()) return 4;
      if (redbtn.held() && yellowbtn.held()) return 99;
      if (missed && (millis() - Timer2 >= 1000)) {
        digitalWrite(RED, flag = !flag);
        Tupdate(Timer2);
      }
      enc.tick();
      if (!mode_time && enc.right()) {
        mode++;
        mode = mode % 4;
        mode_check = 1;
      }
      if (!mode_time && enc.left()) {
        if (mode == 0) mode = 3;
        else
          mode--;
        mode_check = 1;
      }
      if (mode == 1 && enc.click()) {
        if (numberDrinks < 10) numberDrinks++;
      }
      if (mode == 1 && enc.held()) numberDrinks = 0;
      if (mode >= 2 && enc.held()) {
        WTime[EEPcounter] = Timers.WTimeEE;
        RTime[EEPcounter] = Timers.RTimeEE;
        FTime[EEPcounter] = Timers.FTimeEE;
        missedW[EEPcounter] = missedNowW;
        missedF[EEPcounter] = missedNowF;
        // разделить на три параметра
        EEPcounter++;
        EEPcounter = EEPcounter % 4;
        EEPROM.put(5, WTime);
        EEPROM.put(13, RTime);
        EEPROM.put(21, FTime);
        EEPROM.put(29, missedW);
        EEPROM.put(33, missedF);
        EEPROM.put(37, EEPcounter);
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
        mode_time = 1;
        mode_check = 1;
      } else if (mode_time && enc.click()) {
        mode_time = 0;
        mode_check = 1;
      }
      if (mode_time && mode == 2 && enc.leftH()) {
        if (arrowPosition != 4) FTime[arrowPosition] = FTime[arrowPosition] - 10;
        else
          Timers.FTimeEE = Timers.FTimeEE - 10;
      }
      if (mode_time && mode == 2 && enc.rightH()) {
        if (arrowPosition != 4) FTime[arrowPosition] = FTime[arrowPosition] + 10;
        else
          Timers.FTimeEE = Timers.FTimeEE + 10;
      }
      if (mode_time && mode == 3 && enc.leftH()) {
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
      if (mode_time && mode == 4 && enc.leftH()) {
        if (arrowPosition != 4) {
          if (!arrow) missedW[arrowPosition] = missedW[arrowPosition] - 1;
          else
            missedF[arrowPosition] = missedF[arrowPosition] - 1;
        } else {
          if (!arrow) missedNowW = missedNowW - 1;
          else
            missedNowF = missedNowF - 1;
        }
      }
      if (mode_time && mode == 4 && enc.rightH()) {
        if (arrowPosition != 4) {
          if (!arrow) missedW[arrowPosition] = missedW[arrowPosition] + 1;
          else
            missedF[arrowPosition] = missedF[arrowPosition] + 1;
        } else {
          if (!arrow) missedNowW = missedNowW + 1;
          else
            missedNowF = missedNowF + 1;
        }
      }
    }
    Tupdate(Timer1);
    secs++;
    secCounter++;
    if (secs >= 60) {
      secs = secs % 60;
      mins++;
      timeL++;
      Timers.WTimeEE++;
    }
    if (mins >= 60) {
      mins = mins % 60;
      hours++;
    }
    if (hours >= 10) {
      hours = 0;
    }
    // УБРАТЬ ЛИШНИЙ ХЛАМ И ОСТАВИТЬ ТОЛЬКО SECS MINS И HOURS
    if ((missed && timeL - timeL0 >= RepeatedTimer) || (!missed && timeL >= WorkTimer)) {
      byte customCharAlert[] = { B01110, B10001, B10101, B10101, B10001, B10101, B10001, B01110 };
      switches(0, mode % 2, &hours, &mins, &secs, customCharAlert);
      digitalWrite(RED, 1);

      for (int i = 0; i <= 10; i++)  // 10 раз повторяем манипуляции с зуммером
      {
        Timer2 = millis();
        while (millis() - Timer2 < 1000)  // ждем одну секунду, постоянно проверяя, нажата ли кнопка
        {
          yellowbtn.tick();
          redbtn.tick();
          if (redbtn.click()) return 4;
          if (yellowbtn.click()) return 3;
        }
        secs++;
        flag1 = !flag1;
        if (secs >= 60) {
          secs = secs % 60;
          mins++;
          timeL++;
          Timers.WTimeEE++;
        }
        if (mins >= 60) {
          mins = mins % 60;
          hours++;
        }
        if (hours >= 10) {
          hours = 0;
        }
        switches(0, mode % 2, &hours, &mins, &secs, customCharAlert);
        switch (flag1) {
          case 0:
            tone(BUZ, 900);
            break;
          case 1:
            noTone(BUZ);
            break;
        }
      }
      missed = 1;
      missedNowW++;
      timeL0 = timeL;
    }
    if (Timers.STimeEE >= reminderDrinkVariable) {
      for (byte i = 0; i < 2; i++) {
        digitalWrite(RED, 1);
        tone(BUZ, 600);
        Tupdate(Timer1);
        delay(250);
        noTone(BUZ);
        digitalWrite(RED, 0);
        delay(250);
      }
      secs++;
      lcd.setCursor(0, 1);
      lcd.print("Попейте воду!    ");
      delay(1000);
      secs++;
      delay(1000);
      secs++;
      reminderDrinkVariable = reminderDrinkVariable + Timers.STimeEE;
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
        return 4;  // если кнопку так и никто не нажал, отправляем таймер на режим отдыха =)
      }
      digitalWrite(RED, 0);
      noTone(BUZ);
    }
  }
}