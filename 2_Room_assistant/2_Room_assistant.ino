//-------НАСТРОЙКИ---------
#define _LCD_TYPE 1                // тип lcd дисплея (1 - по шине I2C, 2 - десятиконтактное. (Обязательно указывать ДО подключения библиотеки)
#define BTNSOUNDS 1
#define ReminderOnline 60          // рабочее время, которое будет в параметре "Default Settings"
#define RepeatedReminderOnline 20  // сверхурочное рабочее время, которое будет в параметре "Default Settings"
#define ReminderRelax 20           // время прокрастинации, которое будет в параметре "Default Settings"
#define RepeatedReminderRelax 10   // сверхурочное время прокрастинации, которое будет в параметре "Default Settings"
#define ReminderOff 30             // время отдыха, которое будет в параметре "Default Settings"
#define RepeatedReminderOff 10     // сверхурочное время отдыха, которое будет в параметре "Default Settings"
#define RandomCheckupMin 10        // минимальный параметр времени, когда вас могут "опросить" на продуктивность
#define RandomCheckupMax 50        // максимальный параметр времени, когда вас могут "опросить" на продуктивность
#define randomAlertTime 20         // максимальный параметр времени, который делает уведомления более рандомизированными по времени
#define reminderDrink 60           // время, спустя которое вам напомнят попить воды
#define OnlineTime1 90             // первый шаблон времени (красная кнопка)
#define RelaxTime1 30
#define OffTime1 20
#define OnlineTime2 30             // второй шаблон времени (желтая кнопка)
#define RelaxTime2 15
#define OffTime2 10
//-------НАСТРОЙКИ---------

//-------БИБЛИОТЕКИ---------
#define TIMER_ENABLE
// #define DEBUG_ENABLE
#define DHTTYPE DHT22
// #include <MemoryFree.h>
// #include <LCD_1602_RUS_ALL.h>
// #include <font_LCD_1602_RUS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EncButton2.h>
#include "SIMPCODE.h"
// #include "graph.h"
#include <DHT.h>
// #include <SimpleButton.h>
#include <GyverPower.h>
#include <EEPROM.h>
// #include <PinChangeInterrupt.h>
//-------БИБЛИОТЕКИ---------

//-------ПИНЫ---------
#define BUZ 5
#define S1 10
#define S2 4
#define KEY A3
#define DHTPIN 9
#define REDBUT 3
#define YELLOWBUT 2
#define RED 8
#define SEN_VCC 11
#define BUZ_VCC 12
//-------ПИНЫ---------


EncButton2<EB_ENCBTN> enc(INPUT_PULLUP, S1, S2, KEY);
EncButton2<EB_BTN> redbtn(INPUT_PULLUP, REDBUT);
EncButton2<EB_BTN> yellowbtn(INPUT_PULLUP, YELLOWBUT);

// LCD_1602_RUS<LiquidCrystal_I2C> lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHTTYPE);

// SimpleButton redbtn(REDBUT, 2, 100);
// SimpleButton yellowbtn(YELLOWBUT, 2, 100);

struct {
  int Wtime, Rtime, Ftime;
  uint16_t FTimeEE, WTimeEE, RTimeEE, STimeEE, STimeEERep;
} Timers;

#define strict 0
#define checkups 1
#define sounds 2
#define sounds2 3
struct {
  byte sett = 0b10110000;
  // bool strict = 1;
  // bool checkups = 0;
  uint8_t energySave;
  // bool sounds = 1;
} settings;

uint16_t FTime[4];
uint16_t WTime[4];
uint16_t RTime[4];
uint16_t StatsNowW, StatsNowR, StatsNowF, StatsNowS;

uint8_t missedW[4];
uint8_t missedF[4];
uint8_t missedNowW, missedNowF;

uint16_t Wmore[4];
uint16_t Rmore[4];
uint16_t Fmore[4];
uint16_t WmoreNow, RmoreNow, FmoreNow;

uint8_t drink[7];
uint8_t EEPcounterDrink = 0;

uint8_t EEPcounter = 0;

uint16_t reminderDrinkVariable = reminderDrink;
uint8_t numberDrinks = 0;
volatile uint8_t autoOff = 0;
bool ee_request;
newTimer(TimerEE);