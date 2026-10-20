#pragma once
// Общие объявления: подключается в начале каждого .cpp и в RoomAssistant.ino.

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EncButton2.h>
#include <DHT.h>
#include <GyverPower.h>

#include "Config.h"
#include "Types.h"

// ---------- Устройства (определены в RoomAssistant.ino) ----------
extern EncButton2<EB_ENCBTN> enc;
extern EncButton2<EB_BTN> redbtn;
extern EncButton2<EB_BTN> yellowbtn;
extern LiquidCrystal_I2C lcd;
extern DHT dht;

// ---------- Глобальное состояние (определено в RoomAssistant.ino) ----------
extern Settings settings;
extern int modeInterval[MODE_COUNT];
extern ClockTime modeClock[MODE_COUNT];

extern uint16_t todayTime[MODE_COUNT];
extern uint8_t todayMissed[2];
extern uint16_t todayOvertime[MODE_COUNT];
extern uint8_t numberDrinks;

extern uint16_t histTime[MODE_COUNT][HIST_DAYS];
extern uint8_t histMissed[2][HIST_DAYS];
extern uint16_t histOvertime[MODE_COUNT][HIST_DAYS];
extern uint8_t waterHist[WATER_DAYS];
extern uint8_t histIndex;
extern uint8_t waterIndex;

extern uint16_t onlineSinceAlert;
extern uint16_t sinceMissMin;
extern uint16_t nextDrinkReminder;

extern volatile uint8_t idleSeconds;
extern bool settingsDirty;
extern uint32_t lastSettingsSave;

// ---------- Функции, которые вызываются из других модулей ----------

// Setup.cpp — запуск, заставка, быстрые профили
void loadGlyph(byte slot, const byte* glyphInFlash);
void sleepFor(uint32_t ms);
bool applyQuickProfile();
byte beginRun();

// Menu.cpp — меню настройки
byte setupMode(bool& returnedFromRun);

// ModeCommon.cpp — общая логика режимов
void alarmTone(uint16_t freq, uint16_t duration);
void btnsound();
void initModeRun(ModeRun& run, byte mode);
void buildModeIcon(byte* icon, byte mode, byte secCounter);
void advanceSecond(ModeRun& run);
byte pollModeButtons(byte mode);
void blinkLedIfMissed(const ModeRun& run, uint32_t& ledStamp);
bool alertDue(const ModeRun& run);
byte runAlert(ModeRun& run, byte stateBase);
bool drinkReminderDue();
void runDrinkReminder(ModeRun& run);
byte runPresenceCheck(ModeRun& run, byte stateBase);
bool handleEncoder(UiState& ui);

// ModeNormal.cpp — режимы без глубокого энергосбережения
byte runModeNormal(byte mode);

// ModeEco.cpp — режимы с энергосбережением
byte runModeEco(byte mode);

// Screens.cpp — главные экраны режимов
void drawStatusScreen(byte mode, byte view, byte* icon);
void drawScreen(ModeRun& run, byte* icon);

// Graphs.cpp — графики статистики
byte cursorCount(byte screen);
void editValue(const UiState& ui, int8_t dir);
void drawGraph(const UiState& ui);

// Storage.cpp — EEPROM
void saveSettings();
void loadStorage();
void requestStorageReset();
void saveDay();
