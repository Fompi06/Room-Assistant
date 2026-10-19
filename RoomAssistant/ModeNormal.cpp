#include "RoomAssistant.h"

// Рабочий режим без глубокого энергосбережения (EnergySave: Off или Min).
// Возвращает следующее состояние: STATE_NORMAL + режим или STATE_EXIT_TO_MENU.

byte runModeNormal(byte mode) {
  ModeRun run;
  initModeRun(run, mode);

  byte icon[8];
  uint32_t secondStart = millis();
  uint32_t ledStamp = millis();

  while (true) {
    buildModeIcon(icon, mode, run.secCounter);
    drawScreen(run, icon);
    if (!run.missed) digitalWrite(RED, LOW);

    // Пока идёт секунда, опрашиваем кнопки и энкодер
    while (millis() - secondStart < 1000) {
      const byte next = pollModeButtons(mode);
      if (next != NO_BUTTON) return STATE_NORMAL + next;
      if (redbtn.held() && yellowbtn.held()) return STATE_EXIT_TO_MENU;

      blinkLedIfMissed(run, ledStamp);
      if (handleEncoder(run.ui)) drawScreen(run, icon);
    }
    secondStart = millis();
    advanceSecond(run);

    if (alertDue(run)) {
      const byte next = runAlert(run, STATE_NORMAL);
      if (next != NEXT_NONE) return next;
      secondStart = millis();
    }

    if (drinkReminderDue()) {
      runDrinkReminder(run);
      secondStart = millis();
    }

    if (run.minutes == run.checkupAt) {
      const byte next = runPresenceCheck(run, STATE_NORMAL);
      if (next != NEXT_NONE) return next;
      secondStart = millis();
    }
  }
}
