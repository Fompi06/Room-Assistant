#include "RoomAssistant.h"

// Рабочий режим с энергосбережением (EnergySave: Eco).
// Возвращает следующее состояние: STATE_ECO + режим или STATE_EXIT_TO_MENU.
//
// Нет действий 60 с  — контроллер спит между обновлениями (будят красная и жёлтая кнопки).
// Нет действий 120 с — гаснут экран и подсветка, отключается питание датчика.

const byte IDLE_LIGHT_SLEEP = 60;
const byte IDLE_DISPLAY_OFF = 120;
const uint16_t REDRAW_PERIOD = 1000;  // экран обновляется не чаще раза в секунду

// Прерывание красной и жёлтой кнопок: любое нажатие считается действием пользователя
static void btnTick() {
  if (!digitalRead(REDBUT) || !digitalRead(YELLOWBUT)) {
    btnsound();
    idleSeconds = 0;
  }
}

static void enableWakeInterrupts() {
  attachInterrupt(digitalPinToInterrupt(REDBUT), btnTick, FALLING);
  attachInterrupt(digitalPinToInterrupt(YELLOWBUT), btnTick, FALLING);
}

static void disableWakeInterrupts() {
  detachInterrupt(digitalPinToInterrupt(REDBUT));
  detachInterrupt(digitalPinToInterrupt(YELLOWBUT));
}

static void powerOn() {
  lcd.display();
  lcd.backlight();
  digitalWrite(SEN_VCC, HIGH);
  digitalWrite(BUZ_VCC, HIGH);
}

static void powerOff() {
  lcd.noBacklight();
  lcd.noDisplay();
  digitalWrite(SEN_VCC, LOW);
  digitalWrite(BUZ_VCC, LOW);
}

// Переключает состояние энергосбережения в зависимости от времени бездействия
static void updatePowerState(PowerState& state) {
  if (idleSeconds < IDLE_LIGHT_SLEEP) {
    if (state != POWER_ACTIVE) {  // пользователь вернулся
      disableWakeInterrupts();
      powerOn();
      state = POWER_ACTIVE;
    }
  } else if (state == POWER_ACTIVE) {
    enableWakeInterrupts();
    state = POWER_LIGHT_SLEEP;
  } else if (state == POWER_LIGHT_SLEEP && idleSeconds >= IDLE_DISPLAY_OFF) {
    powerOff();
    state = POWER_DISPLAY_OFF;
  }
}

// Включает экран и сбрасывает счётчик бездействия (перед сигналом или проверкой)
static void wakeUp(PowerState& state) {
  idleSeconds = 0;
  if (state != POWER_ACTIVE) {
    disableWakeInterrupts();
    powerOn();
    state = POWER_ACTIVE;
  }
}

byte runModeEco(byte mode) {
  ModeRun run;
  initModeRun(run, mode);

  PowerState powerState = POWER_ACTIVE;
  byte icon[8];
  uint32_t secondStamp = millis();
  uint32_t redrawStamp = millis();
  uint32_t ledStamp = millis();
  bool redrawDue = true;

  while (true) {
    buildModeIcon(icon, mode, run.secCounter);
    if (redrawDue && powerState != POWER_DISPLAY_OFF) {
      redrawDue = false;
      redrawStamp = millis();
      drawScreen(run, icon);
    }
    if (!run.missed) digitalWrite(RED, LOW);

    if (powerState != POWER_ACTIVE) {
      sleepFor(16);  // пока спим, кнопки опрашивает прерывание
    } else {
      delay(10);
      const byte next = pollModeButtons(mode);
      if (next != NO_BUTTON) return STATE_ECO + next;
      if (redbtn.held() && yellowbtn.held()) return STATE_EXIT_TO_MENU;
      delay(10);
      if (handleEncoder(run.ui)) redrawDue = true;
    }
    blinkLedIfMissed(run, ledStamp);

    if (millis() - secondStamp >= 1000) {
      secondStamp += 1000;
      advanceSecond(run);
      if (idleSeconds < 255) idleSeconds++;
    }
    if (!redrawDue && millis() - redrawStamp >= REDRAW_PERIOD) redrawDue = true;
    updatePowerState(powerState);

    if (alertDue(run)) {
      wakeUp(powerState);
      const byte next = runAlert(run, STATE_ECO);
      if (next != NEXT_NONE) return next;
      secondStamp += ALERT_SECONDS * 1000UL;  // эти секунды уже учтены внутри runAlert
      redrawDue = true;
    }

    if (drinkReminderDue()) {
      wakeUp(powerState);
      runDrinkReminder(run);
      secondStamp += 3000;  // три секунды учтены внутри runDrinkReminder
      redrawDue = true;
    }

    if (run.minutes == run.checkupAt) {
      wakeUp(powerState);
      const byte next = runPresenceCheck(run, STATE_ECO);
      if (next != NEXT_NONE) return next;
      redrawDue = true;
    }
  }
}
