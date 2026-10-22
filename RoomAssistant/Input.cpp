#include "RoomAssistant.h"

// Энкодер.
// Повороты ловятся двумя способами: в прерывании по изменению пинов S1/S2 и при обычном
// опросе в цикле. Каждый поворот сразу складывается в счётчик, поэтому щелчки не теряются,
// даже если программа в это время долго выводит что-то на экран.

static volatile int8_t turns;         // повороты без нажатия: + вправо, − влево
static volatile int8_t turnsPressed;  // повороты с нажатой кнопкой

// Переносит событие поворота из библиотеки в счётчики. Вызывать при запрещённых прерываниях.
static void collectTurn() {
  switch (enc.getState()) {
    case 1: turns--; break;
    case 2: turns++; break;
    case 3: turnsPressed--; break;
    case 4: turnsPressed++; break;
    default: return;
  }
  enc.resetState();
  idleSeconds = 0;  // поворот — это действие пользователя (будит режим Eco)
}

ISR(PCINT0_vect) {  // S1 = D10 = PB2
  enc.tickISR();
  collectTurn();
}

ISR(PCINT2_vect) {  // S2 = D4 = PD4
  enc.tickISR();
  collectTurn();
}

void encoderBegin() {
  PCMSK0 |= bit(PCINT2);   // D10
  PCMSK2 |= bit(PCINT20);  // D4
  PCIFR = bit(PCIF0) | bit(PCIF2);
  PCICR |= bit(PCIE0) | bit(PCIE2);
}

void encTick() {
  noInterrupts();
  enc.tick();
  collectTurn();
  interrupts();
}

int8_t takeTurns() {
  noInterrupts();
  const int8_t value = turns;
  turns = 0;
  interrupts();
  return value;
}

int8_t takeTurnsPressed() {
  noInterrupts();
  const int8_t value = turnsPressed;
  turnsPressed = 0;
  interrupts();
  return value;
}

void clearTurns() {
  takeTurns();
  takeTurnsPressed();
}
