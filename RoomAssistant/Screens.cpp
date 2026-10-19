#include "RoomAssistant.h"

// Главные экраны режима: таймер в верхней строке, внизу — температура, вода или сообщение.

// Элементы полосы воды: левый край, середина, правый край; для каждого — полный и пустой
const byte WATER_GLYPHS[6][8] PROGMEM = {
  { 0b01111, 0b10000, 0b10111, 0b10111, 0b10111, 0b10111, 0b10000, 0b01111 },  // 0: левый, полный
  { 0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111 },  // 1: левый, пустой
  { 0b11111, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000, 0b11111 },  // 2: средний, полный
  { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111 },  // 3: средний, пустой
  { 0b11110, 0b00001, 0b11101, 0b11101, 0b11101, 0b11101, 0b00001, 0b11110 },  // 4: правый, полный
  { 0b11110, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b11110 },  // 5: правый, пустой
};

const byte WATER_CELLS = 10;  // цель — 10 стаканов

// Полоса из 10 ячеек и счётчик «n/10»
static void drawWaterBar(byte glasses) {
  for (byte i = 0; i < 6; i++) loadGlyph(i, WATER_GLYPHS[i]);

  lcd.setCursor(0, 1);
  for (byte i = 0; i < WATER_CELLS; i++) {
    const byte base = (i == 0) ? 0 : (i == WATER_CELLS - 1 ? 4 : 2);
    lcd.write(base + (i < glasses ? 0 : 1));
  }
  lcd.print(' ');
  if (glasses < 10) lcd.print(' ');
  lcd.print(glasses);
  lcd.print(F("/10"));
}

static void drawClimate() {
  const float temp = dht.readTemperature();
  const float hum = dht.readHumidity();
  lcd.setCursor(0, 1);
  lcd.print(F("t:"));
  lcd.print(temp, 1);
  lcd.write(223);  // знак градуса
  lcd.print(F("C B:"));
  lcd.print(hum, 1);
  lcd.print('%');
}

// Значок + название режима + время режима (ч:мм:сс), затем нижняя строка по view
void drawStatusScreen(byte mode, byte view, byte* icon) {
  lcd.createChar(7, icon);
  lcd.setCursor(0, 0);
  lcd.write(7);
  switch (mode) {
    case MODE_WORK:  lcd.print(F("Work:   ")); break;
    case MODE_RELAX: lcd.print(F("Relax: ")); break;
    default:         lcd.print(F("Offline:")); break;
  }
  const ClockTime& clock = modeClock[mode];
  lcd.print(clock.hours);
  lcd.print(':');
  if (clock.minutes < 10) lcd.print('0');
  lcd.print(clock.minutes);
  lcd.print(':');
  if (clock.seconds < 10) lcd.print('0');
  lcd.print(clock.seconds);
  lcd.print(' ');

  switch (view) {
    case VIEW_CLIMATE:
      drawClimate();
      break;
    case VIEW_WATER:
      drawWaterBar(numberDrinks);
      break;
    case VIEW_ALERT_MOVE:
      lcd.setCursor(0, 1);
      lcd.print(F("Time to warm up!"));
      break;
    case VIEW_ALERT_WORK:
      lcd.setCursor(0, 1);
      lcd.print(F(" Time to work!  "));
      break;
  }
}

// Рисует текущий экран режима
void drawScreen(ModeRun& run, byte* icon) {
  UiState& ui = run.ui;
  if (ui.needClear) {
    lcd.clear();
    ui.needClear = false;
  }
  switch (ui.screen) {
    case SCR_MAIN:  drawStatusScreen(run.mode, VIEW_CLIMATE, icon); break;
    case SCR_WATER: drawStatusScreen(run.mode, VIEW_WATER, icon); break;
    default:        drawGraph(ui); break;
  }
}
