#include "RoomAssistant.h"

// Графики статистики на LCD 1602.
//
// «День» на графике выбирается позицией курсора cursor: 0 — самый старый сохранённый день,
// HIST_DAYS - 1 — самый новый, HIST_DAYS — текущий (ещё не сохранённый).
// Для воды то же самое, но дней WATER_DAYS.

const byte NO_COL = 255;          // «скобку не рисовать»
const uint16_t TIME_FULL = 320;   // столько минут — столбик на всю высоту
const uint16_t OVER_FULL = 100;

// ---------- Доступ к статистике по позиции курсора ----------

static byte histSlot(byte cursor) {
  return (histIndex + cursor) % HIST_DAYS;
}

static byte waterSlot(byte cursor) {
  return (waterIndex + cursor) % WATER_DAYS;
}

static uint16_t& timeRef(byte mode, byte cursor) {
  return cursor == HIST_DAYS ? todayTime[mode] : histTime[mode][histSlot(cursor)];
}

static uint8_t& missedRef(byte index, byte cursor) {
  return cursor == HIST_DAYS ? todayMissed[index] : histMissed[index][histSlot(cursor)];
}

static uint16_t& overtimeRef(byte mode, byte cursor) {
  return cursor == HIST_DAYS ? todayOvertime[mode] : histOvertime[mode][histSlot(cursor)];
}

static uint8_t& waterRef(byte cursor) {
  return cursor == WATER_DAYS ? numberDrinks : waterHist[waterSlot(cursor)];
}

// Сколько позиций у курсора на данном экране
byte cursorCount(byte screen) {
  return screen == SCR_WATER_STATS ? WATER_DAYS + 1 : HIST_DAYS + 1;
}

// ---------- Правка значений в подробном виде ----------

static void addClamped16(uint16_t& value, int16_t delta) {
  const int32_t result = (int32_t)value + delta;
  value = result < 0 ? 0 : (result > 65535 ? 65535 : result);
}

static void addClamped8(uint8_t& value, int16_t delta) {
  const int16_t result = (int16_t)value + delta;
  value = result < 0 ? 0 : (result > 255 ? 255 : result);
}

// dir: +1 или -1. Время меняется по 10 минут, вода — по одному стакану.
void editValue(const UiState& ui, int8_t dir) {
  switch (ui.screen) {
    case SCR_ONLINE:
      addClamped16(timeRef(MODE_OFFLINE, ui.cursor), dir * 10);
      break;
    case SCR_WORK_RELAX:
      addClamped16(timeRef(ui.lineSwap ? MODE_RELAX : MODE_WORK, ui.cursor), dir * 10);
      break;
    case SCR_WATER_STATS:
      addClamped8(waterRef(ui.cursor), dir);
      break;
  }
}

// ---------- Примитивы отрисовки ----------

// Глифы для столбиков высотой 1..7 пикселей лежат в ячейках 0..6 CGRAM
static void loadBarGlyphs() {
  byte rows[8] = { 0, 0, 0, 0, 0, 0, 0, 0b11111 };
  for (byte height = 0; height < 7; height++) {
    rows[7 - height] = 0b11111;
    lcd.createChar(height, rows);
  }
}

// Символ для столбика высотой h пикселей (0..8) в одной ячейке
static byte barGlyph(byte h) {
  if (h == 0) return ' ';
  if (h >= 8) return 255;  // сплошной блок
  return h - 1;
}

// Столбик высотой value (0..16 пикселей) в колонке col: нижняя ячейка — 8 пикселей, верхняя — остальное
static void drawBar(byte col, byte value) {
  lcd.setCursor(col, 0);
  lcd.write(barGlyph(value > 8 ? value - 8 : 0));
  lcd.setCursor(col, 1);
  lcd.write(barGlyph(value > 8 ? 8 : value));
}

// Переводит value в высоту столбика 0..16, где full соответствует полной высоте
static byte scaleBar(uint16_t value, uint16_t full) {
  if (value >= full) return 16;
  return (uint32_t)value * 16 / full;
}

static byte capBar(uint16_t value) {
  return value > 16 ? 16 : value;
}

// Скобки вокруг выбранного дня (126 — стрелка вправо, 127 — стрелка влево)
static void drawBrackets(byte left, byte right) {
  for (byte row = 0; row < 2; row++) {
    if (left < 16 && !(left == 0 && row == 0)) {  // ячейка (0, 0) занята значком
      lcd.setCursor(left, row);
      lcd.write(126);
    }
    if (right < 16) {
      lcd.setCursor(right, row);
      lcd.write(127);
    }
  }
}

// ---------- Графики ----------

// Три графика «по два столбика на день»: онлайн/офлайн, работа/отдых, пропущенные
static void drawDayGraph(byte screen, byte cursor) {
  byte bars[HIST_DAYS + 1][2];
  for (byte day = 0; day <= HIST_DAYS; day++) {
    switch (screen) {
      case SCR_ONLINE:
        bars[day][0] = scaleBar(timeRef(MODE_WORK, day) + timeRef(MODE_RELAX, day), TIME_FULL);
        bars[day][1] = scaleBar(timeRef(MODE_OFFLINE, day), TIME_FULL);
        break;
      case SCR_WORK_RELAX:
        bars[day][0] = scaleBar(timeRef(MODE_RELAX, day), TIME_FULL);
        bars[day][1] = scaleBar(timeRef(MODE_WORK, day), TIME_FULL);
        break;
      default:  // SCR_MISSED
        bars[day][0] = capBar(missedRef(0, day));
        bars[day][1] = capBar(missedRef(1, day));
        break;
    }
  }

  switch (screen) {
    case SCR_ONLINE:     loadGlyph(7, ICON_PC); break;
    case SCR_WORK_RELAX: loadGlyph(7, ICON_HUMAN); break;
    default:             loadGlyph(7, ICON_BUTTON); break;
  }
  lcd.setCursor(0, 0);
  lcd.write(7);

  drawBrackets(cursor * 3, cursor * 3 + 3);
  for (byte day = 0; day < HIST_DAYS; day++) {
    drawBar(day * 3 + 1, bars[day][0]);
    drawBar(day * 3 + 2, bars[day][1]);
  }
  drawBar(13, bars[HIST_DAYS][0]);  // текущий день
  drawBar(14, bars[HIST_DAYS][1]);
}

// Время сверх нормы: по три столбика (Work, Relax, Offline) на день.
// Страница 1 — история, страница 2 (курсор на текущем дне) — сегодня.
static void drawOvertimeGraph(byte cursor) {
  const bool todayPage = (cursor == HIST_DAYS);
  lcd.setCursor(0, 0);
  lcd.write('%');
  lcd.setCursor(0, 1);
  lcd.write(todayPage ? '2' : '1');

  if (cursor == 0 || todayPage) drawBrackets(NO_COL, 4);
  else drawBrackets(cursor * 4, cursor * 4 + 4);

  if (todayPage) {
    for (byte mode = 0; mode < MODE_COUNT; mode++) {
      drawBar(1 + mode, scaleBar(todayOvertime[mode], OVER_FULL));
    }
    return;
  }
  for (byte day = 0; day < HIST_DAYS; day++) {
    for (byte mode = 0; mode < MODE_COUNT; mode++) {
      drawBar(day * 4 + 1 + mode, scaleBar(overtimeRef(mode, day), OVER_FULL));
    }
  }
}

// Выпитая вода: по столбику на день, последний — сегодня; столбик во всю высоту — 16 стаканов
static void drawWaterGraph(byte cursor) {
  loadGlyph(7, ICON_DROP);
  lcd.setCursor(0, 0);
  lcd.write(7);

  drawBrackets(cursor * 2, cursor * 2 + 2);
  for (byte day = 0; day < WATER_DAYS; day++) {
    drawBar(day * 2 + 1, capBar(waterRef(day)));
  }
  drawBar(15, capBar(numberDrinks));
}

// ---------- Подробный вид ----------

// Строка «[>]Название значение[суффикс]»; selected — рисует стрелку-указатель
static void drawDetailRow(byte row, bool selected, const __FlashStringHelper* label, uint16_t value,
                   const __FlashStringHelper* suffix) {
  lcd.setCursor(0, row);
  if (selected) lcd.write(126);
  else lcd.print(' ');
  lcd.print(label);
  lcd.print(value);
  if (suffix) lcd.print(suffix);
  lcd.print(F("  "));  // затираем «хвост» от прежнего, более длинного значения
}

static void drawDetail(byte screen, const UiState& ui) {
  const byte c = ui.cursor;
  switch (screen) {
    case SCR_ONLINE:
      drawDetailRow(0, !ui.lineSwap, F("Online:"), timeRef(MODE_WORK, c) + timeRef(MODE_RELAX, c), F("m"));
      drawDetailRow(1, ui.lineSwap, F("Offline:"), timeRef(MODE_OFFLINE, c), F("m"));
      break;
    case SCR_WORK_RELAX:
      drawDetailRow(0, !ui.lineSwap, F("Work:"), timeRef(MODE_WORK, c), F("m"));
      drawDetailRow(1, ui.lineSwap, F("Relax:"), timeRef(MODE_RELAX, c), F("m"));
      break;
    case SCR_MISSED:
      drawDetailRow(0, !ui.lineSwap, F("Work missed:"), missedRef(0, c), nullptr);
      drawDetailRow(1, ui.lineSwap, F("Off. missed:"), missedRef(1, c), nullptr);
      break;
    case SCR_OVERTIME:
      // Три пункта, на экране два: lineSwap сдвигает окно на один пункт
      for (byte row = 0; row < 2; row++) {
        const byte mode = row + (ui.lineSwap ? 1 : 0);
        const __FlashStringHelper* label = (mode == MODE_WORK) ? F("Work over:")
                                         : (mode == MODE_RELAX) ? F("Relax over:")
                                                                : F("Off over:");
        drawDetailRow(row, false, label, overtimeRef(mode, c), F("m"));
      }
      break;
    default:  // SCR_WATER_STATS
      drawDetailRow(0, false, F("Glasses: "), waterRef(c), nullptr);
      break;
  }
}

// Рисует экран статистики (график или подробный вид)
void drawGraph(const UiState& ui) {
  if (ui.detail) {
    drawDetail(ui.screen, ui);
    return;
  }
  loadBarGlyphs();
  switch (ui.screen) {
    case SCR_ONLINE:
    case SCR_WORK_RELAX:
    case SCR_MISSED:
      drawDayGraph(ui.screen, ui.cursor);
      break;
    case SCR_OVERTIME:
      drawOvertimeGraph(ui.cursor);
      break;
    default:
      drawWaterGraph(ui.cursor);
      break;
  }
}
