#include "RoomAssistant.h"

// Логика, общая для обычного (ModeNormal) и энергосберегающего (ModeEco) вариантов режимов.

const int DEFAULT_INTERVAL[MODE_COUNT] = { ReminderOnline, ReminderRelax, ReminderOff };
const int DEFAULT_REPEAT[MODE_COUNT] = { RepeatedReminderOnline, RepeatedReminderRelax, RepeatedReminderOff };

const uint16_t CHECKUP_NEVER = 3000;  // «проверка присутствия не нужна»

// Куда переходим по кнопкам: [текущий режим][красная, жёлтая]
const byte BUTTON_TARGET[MODE_COUNT][2] = {
  { MODE_OFFLINE, MODE_RELAX },   // из Work
  { MODE_OFFLINE, MODE_WORK },    // из Relax
  { MODE_WORK, MODE_RELAX },      // из Offline
};

// ---------- Звук ----------

// Короткий сигнал, если звук включён
void alarmTone(uint16_t freq, uint16_t duration) {
  if (bitRead(settings.flags, FLAG_SOUNDS)) tone(BUZ, freq, duration);
}

// Непрерывный сигнал, если звук включён (выключается через noTone)
static void alarmToneStart(uint16_t freq) {
  if (bitRead(settings.flags, FLAG_SOUNDS)) tone(BUZ, freq);
}

// Звук нажатия. Вызывается и из прерывания, поэтому без delay
void btnsound() {
#if (BTNSOUNDS == 1)
  if (bitRead(settings.flags, FLAG_BTN_SOUNDS)) tone(BUZ, NOTE_D5, 50);
#endif
}

// ---------- Подготовка режима ----------

void initModeRun(ModeRun& run, byte mode) {
  memset(&run, 0, sizeof(run));
  run.mode = mode;
  run.ui.needClear = true;

  const byte randomAlert = bitRead(settings.flags, FLAG_STRICT) ? 0 : random(0, randomAlertTime);
  const bool checkups = bitRead(settings.flags, FLAG_CHECKUPS) && mode == MODE_WORK;
  run.checkupAt = checkups ? random(RandomCheckupMin, RandomCheckupMax) : CHECKUP_NEVER;

  if (mode == MODE_OFFLINE) {
    onlineSinceAlert = 0;
    sinceMissMin = 0;
  }

  int& interval = modeInterval[mode];
  run.repeatAfter = 10;
  if (interval < 0) {  // профиль «по умолчанию»
    interval = DEFAULT_INTERVAL[mode];
    run.repeatAfter = DEFAULT_REPEAT[mode];
  } else if (interval > 50) {
    run.repeatAfter = interval / 5;  // 20% от заданного времени
  } else if (interval == 0) {
    interval = 600;  // время не задано — напоминаний не будет
  }
  run.alertAfter = interval + randomAlert;
  run.halfRepeat = run.repeatAfter >> 1;

  digitalWrite(RED, LOW);
  redbtn.resetState();
  yellowbtn.resetState();
  delay(100);
  noTone(BUZ);
}

// Значок режима с анимацией: меняется каждую секунду
void buildModeIcon(byte* icon, byte mode, byte secCounter) {
  static const byte FRAMES[4][2] = {
    { 0b10101, 0b10001 }, { 0b10111, 0b10001 }, { 0b10101, 0b10101 }, { 0b11101, 0b10001 }
  };
  memcpy_P(icon, ICON_MODE, 8);
  if (mode == MODE_OFFLINE) {
    icon[3] = 0b10001;
    icon[4] = (secCounter % 2 == 0) ? 0b10101 : 0b10001;
  } else {
    const byte frame = (mode == MODE_WORK) ? secCounter % 4 : (secCounter % 8) / 2;
    icon[4] = FRAMES[frame][0];
    icon[5] = FRAMES[frame][1];
  }
}

// ---------- Время ----------

// Вызывается раз в минуту
static void onMinutePassed(ModeRun& run) {
  run.minutes++;
  todayTime[run.mode]++;
  if (run.mode != MODE_OFFLINE) onlineSinceAlert++;
  if (run.mode == MODE_RELAX) run.relaxMinutes++;
  if (run.mode == MODE_OFFLINE) run.offlineMinutes++;

  if (run.missed) {
    sinceMissMin++;
    todayOvertime[run.mode]++;
    if (sinceMissMin == run.halfRepeat) alarmTone(NOTE_GS4, 200);  // сигнал на полпути до повтора
  }
}

// Вызывается раз в секунду: двигает часы режима
void advanceSecond(ModeRun& run) {
  ClockTime& clock = modeClock[run.mode];
  clock.seconds++;
  run.secCounter++;
  if (clock.seconds >= 60) {
    clock.seconds %= 60;
    clock.minutes++;
    onMinutePassed(run);
  }
  if (clock.minutes >= 60) {
    clock.minutes %= 60;
    clock.hours++;
  }
  if (clock.hours >= 10) clock.hours = 0;
}

// ---------- Кнопки ----------

// Возвращает режим, в который нужно перейти по кнопке, или NO_BUTTON
byte pollModeButtons(byte mode) {
  redbtn.tick();
  yellowbtn.tick();
  if (redbtn.click()) {
    btnsound();
    idleSeconds = 0;
    return BUTTON_TARGET[mode][0];
  }
  if (yellowbtn.click()) {
    btnsound();
    idleSeconds = 0;
    return BUTTON_TARGET[mode][1];
  }
  return NO_BUTTON;
}

// Ждёт нажатия кнопки до ms миллисекунд
static byte waitForModeButton(byte mode, uint16_t ms) {
  const uint32_t start = millis();
  while (millis() - start < ms) {
    const byte next = pollModeButtons(mode);
    if (next != NO_BUTTON) return next;
  }
  return NO_BUTTON;
}

// Ждёт нажатия любой из двух кнопок до ms миллисекунд
static bool waitForAnyButton(uint16_t ms) {
  const uint32_t start = millis();
  while (millis() - start <= ms) {
    yellowbtn.tick();
    redbtn.tick();
    if (redbtn.click() || yellowbtn.click()) {
      btnsound();
      return true;
    }
  }
  return false;
}

// Если напоминание пропущено, светодиод мигает раз в секунду
void blinkLedIfMissed(const ModeRun& run, uint32_t& ledStamp) {
  if (run.missed && millis() - ledStamp >= 1000) {
    digitalWrite(RED, !digitalRead(RED));
    ledStamp = millis();
  }
}

// ---------- Напоминания ----------

static uint16_t elapsedMinutes(const ModeRun& run) {
  switch (run.mode) {
    case MODE_WORK:  return onlineSinceAlert;
    case MODE_RELAX: return run.relaxMinutes;
    default:         return run.offlineMinutes;
  }
}

// Пора ли напомнить (первый раз или повторно, если предыдущее пропущено)
bool alertDue(const ModeRun& run) {
  if (run.missed) return sinceMissMin >= run.repeatAfter;
  return elapsedMinutes(run) >= run.alertAfter;
}

// Сигнал напоминания: ALERT_SECONDS секунд мигает светодиод и пищит зуммер.
// Возвращает состояние, в которое перешли по кнопке, или NEXT_NONE.
byte runAlert(ModeRun& run, byte stateBase) {
  const byte view = (run.mode == MODE_OFFLINE) ? VIEW_ALERT_WORK : VIEW_ALERT_MOVE;
  byte icon[8];
  memcpy_P(icon, ICON_BUTTON, 8);

  sinceMissMin = 0;
  onlineSinceAlert = 0;
  drawStatusScreen(run.mode, view, icon);
  digitalWrite(RED, HIGH);

  for (byte i = 0; i < ALERT_SECONDS; i++) {
    const byte next = waitForModeButton(run.mode, 1000);
    if (next != NO_BUTTON) return stateBase + next;
    advanceSecond(run);
    drawStatusScreen(run.mode, view, icon);
    if (i % 2 == 0) alarmToneStart(900);
    else noTone(BUZ);
  }
  noTone(BUZ);

  run.missed = true;
  todayMissed[run.mode == MODE_OFFLINE ? 1 : 0]++;
  run.ui.needClear = true;
  return NEXT_NONE;
}

static uint16_t totalToday() {
  return todayTime[MODE_WORK] + todayTime[MODE_RELAX] + todayTime[MODE_OFFLINE];
}

bool drinkReminderDue() {
  return totalToday() >= nextDrinkReminder;
}

void runDrinkReminder(ModeRun& run) {
  for (byte i = 0; i < 2; i++) {
    digitalWrite(RED, HIGH);
    alarmToneStart(600);
    delay(250);
    noTone(BUZ);
    digitalWrite(RED, LOW);
    delay(250);
  }
  advanceSecond(run);
  lcd.setCursor(0, 1);
  lcd.print(F("Drink water!    "));
  delay(1000);
  advanceSecond(run);
  delay(1000);
  advanceSecond(run);

  nextDrinkReminder = totalToday() + reminderDrink;
  run.ui.needClear = true;
}

// ---------- Проверка присутствия ----------

// Подаёт сигнал около 16 секунд. Возвращает true, если пользователь нажал кнопку.
static bool askPresence() {
  bool pressed = false;
  for (byte i = 0; i <= 20 && !pressed; i++) {
    alarmToneStart(700);
    digitalWrite(RED, HIGH);
    pressed = waitForAnyButton(500);
    noTone(BUZ);
    digitalWrite(RED, LOW);
    if (!pressed) pressed = waitForAnyButton(250);
  }
  return pressed;
}

// Возвращает NEXT_NONE, если пользователь на месте, иначе — переход в режим Offline
byte runPresenceCheck(ModeRun& run, byte stateBase) {
  if (askPresence()) {
    run.checkupAt = run.minutes + random(RandomCheckupMin, RandomCheckupMax);
    run.ui.needClear = true;
    return NEXT_NONE;
  }
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(F("Idle Work"));
  lcd.setCursor(0, 1);
  lcd.print(F("You seem away :("));
  delay(2000);
  return stateBase + MODE_OFFLINE;
}

// ---------- Энкодер ----------

// Стакан воды: на десятом — мелодия
static void addGlass() {
  numberDrinks++;
  if (numberDrinks == 10) {
    alarmTone(NOTE_B5, 150);
    delay(200);
    alarmTone(NOTE_B5, 150);
    delay(200);
    alarmTone(NOTE_D6, 700);
    delay(700);
  }
}

// Опрашивает энкодер и обновляет UiState. Возвращает true, если было событие.
bool handleEncoder(UiState& ui) {
  encTick();
  const int8_t t = takeTurns();          // повороты
  const int8_t tp = takeTurnsPressed();  // повороты с нажатой кнопкой
  const bool clicked = enc.click();
  const bool held = enc.held();
  const bool pressed = enc.getState() != 0;  // кнопка просто нажата — тоже действие
  enc.resetState();
  if (!t && !tp && !clicked && !held && !pressed) return false;
  idleSeconds = 0;

  // Листаем экраны
  if (t && !ui.detail) {
    btnsound();
    ui.screen = ((int)ui.screen + t % SCREEN_COUNT + SCREEN_COUNT) % SCREEN_COUNT;
    ui.needClear = true;
  }
  if (ui.cursor >= cursorCount(ui.screen)) ui.cursor = 0;
  const bool onGraph = ui.screen >= SCR_ONLINE;

  // В подробном виде вращение переключает выбранную строку
  if (t && ui.detail && ui.screen != SCR_WATER_STATS && (t & 1)) {
    ui.lineSwap = !ui.lineSwap;
  }

  // Счётчик воды: клик — плюс стакан, удержание — сброс
  if (ui.screen == SCR_WATER && clicked) {
    btnsound();
    addGlass();
  }
  if (ui.screen == SCR_WATER && held) {
    btnsound();
    numberDrinks = 0;
  }

  // На графиках: удержание — сохранить «день»
  if (onGraph && held) {
    saveDay();
    ui.needClear = true;
  }

  // Поворот с нажатой кнопкой: выбор дня на графике или правка значения в подробном виде
  if (onGraph && tp) {
    btnsound();
    const int8_t dir = tp > 0 ? 1 : -1;
    if (ui.detail) {
      for (int8_t i = 0; i != tp; i += dir) editValue(ui, dir);
    } else {
      const int count = cursorCount(ui.screen);
      ui.cursor = ((int)ui.cursor + tp % count + count) % count;
      ui.needClear = true;
    }
  }

  // Клик на графике открывает и закрывает подробный вид
  if (onGraph && clicked) {
    btnsound();
    ui.detail = !ui.detail;
    ui.lineSwap = false;
    ui.needClear = true;
  }
  return true;
}
