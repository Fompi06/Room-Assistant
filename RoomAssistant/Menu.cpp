#include "RoomAssistant.h"

// Меню настройки.
// Шаги 0..2 — ввод времени Work / Relax / Offline, шаг 3 — параметры устройства.
// Удержание энкодера переходит к следующему шагу, после шага 3 запускается режим Work.

const byte MENU_STEP_SETTINGS = 3;
const byte MENU_ITEMS = 6;
const int MAX_INTERVAL = 599;  // 9 ч 59 мин
const uint16_t SETTINGS_SAVE_PERIOD = 5000;  // не чаще раза в 5 с, чтобы не изнашивать EEPROM

// Экран меню перерисовывается только при изменениях: вывод на LCD по I2C медленный.
static bool menuDirty = true;   // перерисовать весь экран
static bool valueDirty = true;  // перерисовать только строку со значением

// Ввод времени для режима which (MODE_WORK / MODE_RELAX / MODE_OFFLINE)
static void editInterval(byte which) {
  int& value = modeInterval[which];
  const int before = value;
  value += takeTurns() + 10 * takeTurnsPressed();  // поворот с нажатой кнопкой — шаг 10 минут
  if (value > MAX_INTERVAL) value = MAX_INTERVAL;
  if (value < 0) value = -1;
  if (value != before) valueDirty = true;

  if (menuDirty) {
    menuDirty = false;
    valueDirty = true;
    lcd.setCursor(0, 0);
    switch (which) {
      case MODE_WORK:    lcd.print(F("Set online time:")); break;
      case MODE_RELAX:   lcd.print(F("Set relax time: ")); break;
      default:           lcd.print(F("Set offline time")); break;
    }
  }
  if (!valueDirty) return;
  valueDirty = false;

  lcd.setCursor(0, 1);
  if (value < 0) {
    lcd.print(F("Default Settings"));
    return;
  }
  const byte hours = value / 60;
  const byte tens = value / 10 - hours * 6;
  const byte units = value % 10;
  lcd.print(hours);
  lcd.print(F(" hrs : "));
  lcd.print(tens);
  lcd.print(units);
  lcd.print(F(" mins "));
}

// ---------- Параметры устройства ----------

// Звук: 0 — выключен, 1 — только сигналы, 2 — сигналы и звуки нажатий
static byte soundLevel() {
  if (!bitRead(settings.flags, FLAG_SOUNDS)) return 0;
  return bitRead(settings.flags, FLAG_BTN_SOUNDS) ? 2 : 1;
}

static void setSoundLevel(byte level) {
  bitWrite(settings.flags, FLAG_SOUNDS, level >= 1);
  bitWrite(settings.flags, FLAG_BTN_SOUNDS, level >= 2);
}

static void toggleFlag(byte flag) {
  bitWrite(settings.flags, flag, !bitRead(settings.flags, flag));
}

static void showOk(byte row) {
  lcd.setCursor(14, row);
  lcd.print(F("OK"));
  delay(1000);
}

static void activateMenuItem(byte item) {
  switch (item) {
    case 0:
      toggleFlag(FLAG_STRICT);
      settingsDirty = true;
      break;
    case 1:
      toggleFlag(FLAG_CHECKUPS);
      settingsDirty = true;
      break;
    case 2:
      settings.energySave = (settings.energySave + 1) % 3;
      settingsDirty = true;
      break;
    case 3:
      setSoundLevel((soundLevel() + 1) % 3);
      settingsDirty = true;
      break;
    case 4:  // SaveSettings
      if (settingsDirty && millis() - lastSettingsSave >= SETTINGS_SAVE_PERIOD) {
        lastSettingsSave = millis();
        settingsDirty = false;
        saveSettings();
        showOk(0);
      }
      break;
    case 5:  // ResetSettings: применится после перезагрузки
      requestStorageReset();
      showOk(1);
      break;
  }
}

// Рисует одну строку меню; значение выравнивается по правому краю
static void drawMenuRow(byte row, byte item, bool selected) {
  lcd.setCursor(0, row);
  if (selected) lcd.write(7);
  else lcd.print(' ');

  byte used = 1;
  const char* value = "";
  switch (item) {
    case 0:
      used += lcd.print(F("Strict mode:"));
      value = bitRead(settings.flags, FLAG_STRICT) ? "Yes" : "No";
      break;
    case 1:
      used += lcd.print(F("Checks:"));
      value = bitRead(settings.flags, FLAG_CHECKUPS) ? "Yes" : "No";
      break;
    case 2:
      used += lcd.print(F("EnergySave:"));
      value = settings.energySave == ENERGY_OFF ? "Off" : (settings.energySave == ENERGY_MIN ? "Min" : "Eco");
      break;
    case 3:
      used += lcd.print(F("Sounds:"));
      value = soundLevel() == 0 ? "No" : (soundLevel() == 1 ? "Alarm" : "All");
      break;
    case 4:
      used += lcd.print(F("SaveSettings"));
      break;
    case 5:
      used += lcd.print(F("ResetSettings"));
      break;
  }
  const byte valueLen = strlen(value);
  while (used + valueLen < 16) {
    lcd.print(' ');
    used++;
  }
  lcd.print(value);
}

static void settingsMenu(byte& cursor) {
  const int8_t t = takeTurns();
  takeTurnsPressed();  // повороты с нажатой кнопкой здесь не используются
  if (t) {
    int next = cursor + t;
    if (next < 0) next = 0;
    if (next > MENU_ITEMS - 1) next = MENU_ITEMS - 1;
    if (next != cursor) {
      cursor = next;
      btnsound();
      menuDirty = true;
    }
  }
  if (enc.click()) {
    btnsound();
    activateMenuItem(cursor);
    menuDirty = true;
  }

  if (!menuDirty) return;
  menuDirty = false;
  loadGlyph(7, ICON_ARROW);
  const byte firstItem = cursor & ~1;  // на экране сразу два пункта
  drawMenuRow(0, firstItem, firstItem == cursor);
  drawMenuRow(1, firstItem + 1, firstItem + 1 == cursor);
}

// Вызывается из loop() многократно, пока идёт настройка.
// Возвращает STATE_SETUP, пока настройка не закончена, затем — стартовое состояние режима.
byte setupMode(bool& returnedFromRun) {
  static byte step;
  static byte cursor;
  if (returnedFromRun) {
    step = 0;
    returnedFromRun = false;
    menuDirty = true;
  }

  encTick();
  redbtn.tick();
  yellowbtn.tick();
  if (applyQuickProfile()) return beginRun();

  if (enc.held()) {
    step++;
    btnsound();
    menuDirty = true;
  }

  if (step < MODE_COUNT) {
    editInterval(step);
  } else if (step == MENU_STEP_SETTINGS) {
    settingsMenu(cursor);
  } else {
    return beginRun();
  }
  return STATE_SETUP;
}
