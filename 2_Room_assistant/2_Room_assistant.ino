
#define STATS 1               // 0 - отключено, 1 - включено
#define ENERGY_SAVE 0         // 0 - питание от сети, 1 - от батареек


#define _LCD_TYPE 1
#define TIMER_ENABLE
#define DHTTYPE DHT22
#include <LCD_1602_RUS_ALL.h>
#include <font_LCD_1602_RUS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EncButton2.h>
#include "SIMPCODE.h"
// #include "graph.h"
#include <DHT.h>
// #include <SimpleButton.h>
#include <GyverPower.h>
#include <EEPROM.h>


#define BUZ 5
#define S1 10
#define S2 4
#define KEY 2
#define DHTPIN 9
#define REDBUT 3
#define YELLOWBUT 12
#define RED 8
#define GREEN 6

#define ReminderWork 60           // время, спустя которое вам напомнят об отдыхе
#define RepeatedReminderWork 20   // время, спустя которое вам напомнят об отдыхе, если вы уже работаете сверхурочно
#define ReminderRelax 20          // время, спустя которое вам напомнят о работе
#define RepeatedReminderRelax 10  // время, спустя которое вам напомнят о работе, если вы уже отдыхаете слишком долго
#define RandomCheckupMin 10
#define RandomCheckupMax 50
#define randomAlertTime 20
#define reminderDrink 60

EncButton2<EB_ENCBTN> enc(INPUT_PULLUP, S1, S2, KEY);
EncButton2<EB_BTN> redbtn(INPUT_PULLUP, REDBUT);
EncButton2<EB_BTN> yellowbtn(INPUT_PULLUP, YELLOWBUT);  

LCD_1602_RUS<LiquidCrystal_I2C> lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHTTYPE);

// SimpleButton redbtn(REDBUT, 2, 100);
// SimpleButton yellowbtn(YELLOWBUT, 2, 100);

struct {
  float temp;
  float hum;
} dht1;

struct {
  int Wtime;
  int Rtime;
  int Ftime;
  uint16_t FTimeEE;
  uint16_t WTimeEE;
  uint16_t RTimeEE;
  uint16_t STimeEE;
} Timers;

struct {
  bool strict;
  bool checkups;
  byte energySave;
  bool stats;
} settings;

uint16_t FTime[4];
uint16_t WTime[4];
uint16_t RTime[4];

uint8_t missedW[4];
uint8_t missedF[4];

uint8_t mPerW[4];
uint8_t mPerR[4];
uint8_t mPerF[4];

uint16_t StatsNowW;
uint16_t StatsNowR;
uint16_t StatsNowF;
uint16_t StatsNowS;

uint8_t missedNowW;
uint8_t missedNowF;

byte EEPcounter = 0;

uint16_t reminderDrinkVariable = reminderDrink;
byte numberDrinks = 0;
bool ee_request;
bool ee_reset;
newTimer(TimerEE);
newTimer(TimerEEreset);