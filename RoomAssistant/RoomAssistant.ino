/*
  Room Assistant — комнатный помощник на Arduino Nano (ATmega328P).

  Напоминает вовремя размяться и отдохнуть, измеряет температуру и влажность,
  считает выпитую воду и рисует графики активности на LCD 1602.

  Настройки (таймеры, профили, пины) — в файле Config.h.

  Файлы скетча:
    Config.h          настройки и пины
    Types.h           константы, структуры, значки
    RoomAssistant.h   общие объявления для всех модулей
    Setup.cpp         setup(), loop(), заставка, быстрые профили
    Input.cpp         энкодер: опрос в прерывании, счётчики поворотов
    Menu.cpp          меню настройки времени и параметров
    ModeCommon.cpp    общая логика рабочих режимов
    ModeNormal.cpp    рабочие режимы без глубокого энергосбережения
    ModeEco.cpp       рабочие режимы с энергосбережением (Eco)
    Screens.cpp       главные экраны режимов
    Graphs.cpp        графики статистики
    Storage.cpp       сохранение в EEPROM

  Логика лежит в .cpp-файлах, а не в .ino: так Arduino IDE не генерирует
  объявления функций автоматически и не путается в порядке типов.
  В этом файле — только глобальные объекты и переменные.
*/

#include "RoomAssistant.h"

// ---------- Устройства ----------
EncButton2<EB_ENCBTN> enc(INPUT_PULLUP, S1, S2, KEY);
EncButton2<EB_BTN> redbtn(INPUT_PULLUP, REDBUT);
EncButton2<EB_BTN> yellowbtn(INPUT_PULLUP, YELLOWBUT);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // адрес 0x27; у некоторых адаптеров 0x3F
DHT dht(DHTPIN, DHTTYPE);

// ---------- Настройки и интервалы ----------
Settings settings;
int modeInterval[MODE_COUNT];  // заданное время режима, мин. Меньше нуля — «по умолчанию»

// ---------- Часы режимов (сохраняются при переключении режимов) ----------
ClockTime modeClock[MODE_COUNT];

// ---------- Статистика текущего «дня» (хранится в ОЗУ) ----------
uint16_t todayTime[MODE_COUNT];      // минут в каждом режиме
uint8_t todayMissed[2];              // пропущенные уведомления: [0] — работа/отдых, [1] — офлайн
uint16_t todayOvertime[MODE_COUNT];  // минут сверх нормы после пропущенного уведомления
uint8_t numberDrinks;                // стаканов воды

// ---------- История (хранится в EEPROM) ----------
uint16_t histTime[MODE_COUNT][HIST_DAYS];
uint8_t histMissed[2][HIST_DAYS];
uint16_t histOvertime[MODE_COUNT][HIST_DAYS];
uint8_t waterHist[WATER_DAYS];
uint8_t histIndex;   // в какую ячейку истории писать следующей (она же — самая старая)
uint8_t waterIndex;

// ---------- Таймеры напоминаний ----------
uint16_t onlineSinceAlert;           // минут Work+Relax с последнего напоминания
uint16_t sinceMissMin;               // минут с последнего пропущенного напоминания
uint16_t nextDrinkReminder = reminderDrink;  // при какой сумме минут напомнить про воду

// ---------- Прочее ----------
volatile uint8_t idleSeconds;        // секунд без действий пользователя (для режима Eco)
bool settingsDirty;                  // настройки изменены и ещё не сохранены
uint32_t lastSettingsSave;           // когда настройки сохраняли в последний раз
