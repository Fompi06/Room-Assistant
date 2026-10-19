#include <EEPROM.h>
#include "RoomAssistant.h"

// Сохранение настроек и истории в EEPROM.
// Раскладка адресов описана в Types.h (EE_*).

void saveSettings() {
  EEPROM.put(EE_SETTINGS, settings);
}

static void saveHistory() {
  EEPROM.put(EE_HIST_TIME, histTime);
  EEPROM.put(EE_HIST_MISSED, histMissed);
  EEPROM.put(EE_HIST_OVER, histOvertime);
  EEPROM.put(EE_WATER, waterHist);
  EEPROM.put(EE_HIST_INDEX, histIndex);
  EEPROM.put(EE_WATER_INDEX, waterIndex);
}

// При первом запуске записывает в EEPROM значения по умолчанию, иначе читает сохранённое
void loadStorage() {
  if (EEPROM.read(EE_MAGIC) != EE_MAGIC_VALUE) {
    saveSettings();
    saveHistory();
    EEPROM.update(EE_MAGIC, EE_MAGIC_VALUE);
    return;
  }
  EEPROM.get(EE_SETTINGS, settings);
  EEPROM.get(EE_HIST_TIME, histTime);
  EEPROM.get(EE_HIST_MISSED, histMissed);
  EEPROM.get(EE_HIST_OVER, histOvertime);
  EEPROM.get(EE_WATER, waterHist);
  EEPROM.get(EE_HIST_INDEX, histIndex);
  EEPROM.get(EE_WATER_INDEX, waterIndex);
}

// Сброс: при следующем включении EEPROM будет инициализирована заново
void requestStorageReset() {
  EEPROM.update(EE_MAGIC, 0);
}

// Закрывает текущий «день»: переносит статистику в историю и сохраняет её
void saveDay() {
  for (byte m = 0; m < MODE_COUNT; m++) {
    histTime[m][histIndex] = todayTime[m];
    histOvertime[m][histIndex] = todayOvertime[m];
  }
  histMissed[0][histIndex] = todayMissed[0];
  histMissed[1][histIndex] = todayMissed[1];
  waterHist[waterIndex] = numberDrinks;

  histIndex = (histIndex + 1) % HIST_DAYS;
  waterIndex = (waterIndex + 1) % WATER_DAYS;
  saveHistory();

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(F("Data saved!"));
  if (bitRead(settings.flags, FLAG_SOUNDS)) {
    tone(BUZ, NOTE_A4, 200);
    delay(300);
    tone(BUZ, NOTE_A4, 200);
    delay(700);
  } else {
    delay(1000);
  }
}
