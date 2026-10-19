#pragma once
#include <Arduino.h>

// ---------- Режимы работы (индексы в массивах) ----------
const byte MODE_WORK    = 0;  // работа
const byte MODE_RELAX   = 1;  // прокрастинация
const byte MODE_OFFLINE = 2;  // отдых вдали от компьютера
const byte MODE_COUNT   = 3;

// ---------- Состояния, между которыми переключается loop() ----------
// Функции режимов возвращают STATE_NORMAL + режим или STATE_ECO + режим.
const byte STATE_SETUP        = 0;   // меню настройки
const byte STATE_NORMAL       = 1;   // 1..3: режимы без глубокого энергосбережения
const byte STATE_ECO          = 11;  // 11..13: режимы с энергосбережением
const byte STATE_EXIT_TO_MENU = 99;  // выйти из режима обратно в меню
const byte NEXT_NONE          = 255; // «остаться в текущем режиме»
const byte NO_BUTTON          = 255; // «кнопка не нажата»
const byte ALERT_SECONDS      = 7;   // длительность сигнала напоминания, с

// ---------- Настройки пользователя (хранятся в EEPROM) ----------
const byte FLAG_STRICT     = 0;  // напоминания точно по таймеру, без случайной добавки
const byte FLAG_CHECKUPS   = 1;  // случайные проверки присутствия в режиме Work
const byte FLAG_SOUNDS     = 2;  // звуковые сигналы
const byte FLAG_BTN_SOUNDS = 3;  // звуки нажатий (имеет смысл только вместе с FLAG_SOUNDS)

const byte ENERGY_OFF = 0;
const byte ENERGY_MIN = 1;
const byte ENERGY_ECO = 2;

struct Settings {
  byte flags;
  uint8_t energySave;
};

// ---------- Экраны, между которыми листает энкодер ----------
const byte SCR_MAIN         = 0;  // таймер + температура и влажность
const byte SCR_WATER        = 1;  // таймер + счётчик воды
const byte SCR_ONLINE       = 2;  // график: онлайн / офлайн
const byte SCR_WORK_RELAX   = 3;  // график: работа / отдых
const byte SCR_MISSED       = 4;  // график: пропущенные уведомления
const byte SCR_OVERTIME     = 5;  // график: время сверх нормы
const byte SCR_WATER_STATS  = 6;  // график: вода по дням
const byte SCREEN_COUNT     = 7;

// Что показывать на «главном» экране режима
const byte VIEW_CLIMATE    = 0;  // температура и влажность
const byte VIEW_WATER      = 1;  // полоса воды
const byte VIEW_ALERT_MOVE = 2;  // «Time to warm up!»
const byte VIEW_ALERT_WORK = 3;  // «Time to work!»

// ---------- Статистика ----------
const byte HIST_DAYS  = 4;  // сохранённых «дней» (плюс текущий)
const byte WATER_DAYS = 7;  // сохранённых дней по воде (плюс текущий)

// ---------- Ноты ----------
const uint16_t NOTE_GS4 = 415;
const uint16_t NOTE_A4  = 440;
const uint16_t NOTE_C5  = 523;
const uint16_t NOTE_D5  = 587;
const uint16_t NOTE_F5  = 698;
const uint16_t NOTE_A5  = 880;
const uint16_t NOTE_B5  = 988;
const uint16_t NOTE_D6  = 1175;

// ---------- Раскладка EEPROM ----------
const int EE_MAGIC        = 0;   // признак «EEPROM уже инициализирована»
const int EE_SETTINGS     = 1;   // Settings (2 байта)
const int EE_HIST_TIME    = 3;   // uint16_t[MODE_COUNT][HIST_DAYS]
const int EE_HIST_MISSED  = 27;  // uint8_t[2][HIST_DAYS]
const int EE_HIST_OVER    = 35;  // uint16_t[MODE_COUNT][HIST_DAYS]
const int EE_WATER        = 59;  // uint8_t[WATER_DAYS]
const int EE_HIST_INDEX   = 66;
const int EE_WATER_INDEX  = 67;
const byte EE_MAGIC_VALUE = 254;

// ---------- Состояние, общее для всех экранов режима ----------
struct UiState {
  byte screen;      // текущий экран (SCR_*)
  byte cursor;      // выбранный «день» на графике
  bool detail;      // открыт подробный вид графика
  bool lineSwap;    // какая строка выбрана в подробном виде
  bool needClear;   // перед следующей отрисовкой очистить экран
};

// ---------- Часы одного режима ----------
struct ClockTime {
  byte hours;
  byte minutes;
  byte seconds;
};

// ---------- Состояние одного запуска режима ----------
struct ModeRun {
  byte mode;
  uint16_t alertAfter;     // через сколько минут первое напоминание
  uint16_t repeatAfter;    // через сколько минут повторять, если напоминание пропущено
  uint8_t halfRepeat;      // на какой минуте повтора подать «предупредительный» сигнал
  uint16_t checkupAt;      // на какой минуте проверить присутствие
  uint16_t minutes;        // сколько минут прошло с начала этого запуска
  uint16_t relaxMinutes;   // то же, но только для режима Relax
  uint16_t offlineMinutes; // то же, но только для режима Offline
  byte secCounter;         // счётчик секунд для анимации значка
  bool missed;             // напоминание уже было, а пользователь не отреагировал
  UiState ui;
};

// ---------- Состояние энергосбережения (режим Eco) ----------
enum PowerState {
  POWER_ACTIVE,       // всё включено
  POWER_LIGHT_SLEEP,  // нет действий 60 с: контроллер спит между обновлениями
  POWER_DISPLAY_OFF   // нет действий 120 с: экран и датчик обесточены
};

// ---------- Значки для LCD (8 строк по 5 бит) ----------
const byte ICON_MODE[8]   PROGMEM = { 0b00000, 0b00000, 0b01110, 0b10101, 0b10101, 0b10001, 0b01110, 0b00000 };
const byte ICON_BUTTON[8] PROGMEM = { 0b01110, 0b10001, 0b10101, 0b10101, 0b10001, 0b10101, 0b10001, 0b01110 };
const byte ICON_PC[8]     PROGMEM = { 0x00, 0x1F, 0x11, 0x11, 0x1F, 0x04, 0x0E, 0x00 };
const byte ICON_HUMAN[8]  PROGMEM = { 0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A };
const byte ICON_DROP[8]   PROGMEM = { 0b00100, 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01110 };
const byte ICON_ARROW[8]  PROGMEM = { 0b00000, 0b00100, 0b00010, 0b11101, 0b11101, 0b00010, 0b00100, 0b00000 };
