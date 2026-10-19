#include "RoomAssistant.h"

// ---------- Глифы кириллицы для заставки (в ПЗУ дисплея русских букв нет) ----------
// Номер глифа = номер ячейки CGRAM дисплея
const byte GL_P = 0, GL_I = 1, GL_V = 2, GL_T = 3, GL_M = 4, GL_N = 5, GL_Y = 6, GL_J = 7;
const byte GL_SCH = 2;  // «щ» занимает ту же ячейку, что «в» (она на втором экране не нужна)

const byte SPLASH_GLYPHS[8][8] PROGMEM = {
  { 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b00000 },  // П
  { 0b00000, 0b00000, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // и
  { 0b00000, 0b00000, 0b11110, 0b10001, 0b11110, 0b10001, 0b11110, 0b00000 },  // в
  { 0b00000, 0b00000, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000 },  // т
  { 0b00000, 0b00000, 0b10001, 0b11011, 0b10101, 0b10001, 0b10001, 0b00000 },  // м
  { 0b00000, 0b00000, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b00000 },  // н
  { 0b00000, 0b00000, 0b10001, 0b10001, 0b11101, 0b10011, 0b11101, 0b00000 },  // ы
  { 0b01010, 0b00100, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // й
};
const byte GLYPH_SCH[8] PROGMEM = { 0b00000, 0b00000, 0b10101, 0b10101, 0b10101, 0b10101, 0b11111, 0b00001 };  // щ

// Загружает глиф из флеш-памяти в ячейку CGRAM дисплея
void loadGlyph(byte slot, const byte* glyphInFlash) {
  byte buf[8];
  memcpy_P(buf, glyphInFlash, 8);
  lcd.createChar(slot, buf);
}

// Выводит символ и ждёт — получается эффект «печатающегося» текста
static void typeChar(byte code) {
  lcd.write(code);
  delay(100);
}

static void playSplash() {
  for (byte i = 0; i < 8; i++) loadGlyph(i, SPLASH_GLYPHS[i]);

  // «Привет!» / «Hello!»
  alarmTone(NOTE_C5, 200);
  lcd.setCursor(4, 0);
  typeChar(GL_P);
  typeChar('p');
  typeChar(GL_I);
  alarmTone(NOTE_C5, 200);
  typeChar(GL_V);
  typeChar('e');
  typeChar(GL_T);
  alarmTone(NOTE_A5, 300);
  typeChar('!');

  lcd.setCursor(5, 1);
  const char hello[] = "Hello!";
  for (byte i = 0; i < 6; i++) {
    typeChar(hello[i]);
    if (i == 2) alarmTone(NOTE_F5, 300);
  }
  delay(750);

  // «Комнатный помощник»
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(F("Ko"));  // «К» и «о» совпадают с латинскими
  delay(100);
  typeChar(GL_M);
  typeChar(GL_N);
  typeChar('a');
  typeChar(GL_T);
  typeChar(GL_N);
  typeChar(GL_Y);
  typeChar(GL_J);

  loadGlyph(GL_SCH, GLYPH_SCH);
  lcd.setCursor(4, 1);
  typeChar(GL_P);
  typeChar('o');
  typeChar(GL_M);
  typeChar('o');
  typeChar(GL_SCH);
  typeChar(GL_N);
  typeChar(GL_I);
  lcd.print('k');  // «к» похожа на латинскую k
  delay(750);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(F("by Fompi"));
  delay(750);
}

void setup() {
  power.hardwareEnable(PWR_ALL);
  power.calibrate(8000);
  power.correctMillis(true);

  pinMode(SEN_VCC, OUTPUT);
  pinMode(BUZ_VCC, OUTPUT);
  pinMode(RED, OUTPUT);
  digitalWrite(SEN_VCC, HIGH);
  digitalWrite(BUZ_VCC, HIGH);

  loadStorage();
  randomSeed(analogRead(0));
  dht.begin();
  lcd.init();
  lcd.backlight();
  playSplash();
}

// ---------- Быстрые профили ----------

// Показывает название профиля и его времена: Work / Relax / Offline
static void showProfile(const __FlashStringHelper* title, int work, int relax, int offline) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(title);
  lcd.setCursor(0, 1);
  lcd.print(F("W"));
  lcd.print(work);
  lcd.print(F(" R"));
  lcd.print(relax);
  lcd.print(F(" O"));
  lcd.print(offline);
  lcd.noBlink();
  delay(750);
}

static void applyProfile(const __FlashStringHelper* title, int work, int relax, int offline) {
  btnsound();
  modeInterval[MODE_WORK] = work;
  modeInterval[MODE_RELAX] = relax;
  modeInterval[MODE_OFFLINE] = offline;
  showProfile(title, work, relax, offline);
}

// Если удерживается красная и/или жёлтая кнопка — применяет профиль и возвращает true
bool applyQuickProfile() {
  if (redbtn.hold() && yellowbtn.hold()) {
    applyProfile(F("Notif. disabled"), 600, 600, 600);  // 10 часов — уведомления практически отключены
    return true;
  }
  if (redbtn.hold()) {
    applyProfile(F("Mode 1 activated"), OnlineTime1, RelaxTime1, OffTime1);
    return true;
  }
  if (yellowbtn.hold()) {
    applyProfile(F("Mode 2 activated"), OnlineTime2, RelaxTime2, OffTime2);
    return true;
  }
  return false;
}

// Применяет уровень энергосбережения и возвращает стартовое состояние (режим Work)
byte beginRun() {
  if (settings.energySave >= ENERGY_MIN) {
    power.hardwareDisable(PWR_SPI | PWR_UART0 | PWR_UART2 | PWR_UART1 | PWR_UART3);
  }
  return (settings.energySave == ENERGY_ECO ? STATE_ECO : STATE_NORMAL) + MODE_WORK;
}

// Заготовка экрана меню: первый шаг настройки (время работы)
static void drawMenuBase() {
  lcd.display();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("Set online time:"));
  lcd.setCursor(0, 1);
  lcd.print(F("0 hrs : 00 mins"));
}

void loop() {
  static bool returnedFromRun;  // true — вернулись из рабочего режима удержанием двух кнопок
  byte state = STATE_SETUP;

  digitalWrite(SEN_VCC, HIGH);
  if (returnedFromRun) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Exit to menu"));
  }

  // 2 секунды на выбор быстрого профиля: удерживайте энкодер, красную или жёлтую кнопку
  lcd.setCursor(0, 0);
  lcd.blink();
  const uint32_t windowStart = millis();
  while (millis() - windowStart <= 2000) {
    enc.tick();
    redbtn.tick();
    yellowbtn.tick();
    if (enc.hold()) {
      applyProfile(F("Default Settings"), ReminderOnline, ReminderRelax, ReminderOff);
      // Отрицательное время — признак профиля «по умолчанию»: значения подставятся при старте режима
      modeInterval[MODE_WORK] = modeInterval[MODE_RELAX] = modeInterval[MODE_OFFLINE] = -1;
      state = beginRun();
      break;
    }
    if (applyQuickProfile()) {
      state = beginRun();
      break;
    }
  }
  lcd.noBlink();

  drawMenuBase();
  // Пауза 3 с в спящем режиме: экран и датчик на это время обесточены
  digitalWrite(SEN_VCC, LOW);
  lcd.noBacklight();
  lcd.noDisplay();
  power.sleepDelay(3000);
  digitalWrite(SEN_VCC, HIGH);
  delay(100);
  lcd.backlight();
  lcd.display();

  while (true) {
    if (state == STATE_SETUP) {
      state = setupMode(returnedFromRun);
    } else if (state >= STATE_ECO) {
      state = runModeEco(state - STATE_ECO);
    } else {
      state = runModeNormal(state - STATE_NORMAL);
    }

    if (state == STATE_EXIT_TO_MENU) {
      returnedFromRun = true;
      return;  // loop() вызовется снова — снова откроется окно быстрого выбора профиля
    }
  }
}
