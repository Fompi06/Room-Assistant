#pragma once
#include <Arduino.h>

class SimpleButton {
  public:
    SimpleButton(byte pin, byte mode = 0, deb = 100) {
      _pin = pin;
      _mode = mode;
      _deb = deb;
  }

    void begin(){
      switch(_mode){
      case 0:
      pinMode(_pin, INPUT);
      break;
      case 1:
      pinMode(_pin, INPUT);
      break;
      case 2:
      pinMode(_pin, INPUT_PULLUP);
      break;
      }
    }

    bool clicked(){
      switch(_mode) {
        case 0:
        bool btnState = digitalRead(_pin);
        if (!btnState && !_flag && millis() - _tmr >= _deb) {
        _flag = true;
        _tmr = millis();
        return true;
        }
        if (btnState && _flag && millis() - _tmr >= _deb) {
        _flag = false;
        _tmr = millis();
        }
        return false;
        break;

        case 1:
        bool btnState = !digitalRead(_pin);
        if (!btnState && !_flag && millis() - _tmr >= _deb) {
        _flag = true;
        _tmr = millis();
        return true;
        }
        if (btnState && _flag && millis() - _tmr >= _deb) {
        _flag = false;
        _tmr = millis();
        }
        return false;
        break;

        case 2:
        bool btnState = !digitalRead(_pin);
        if (!btnState && !_flag && millis() - _tmr >= _deb) {
        _flag = true;
        _tmr = millis();
        return true;
        }
        if (btnState && _flag && millis() - _tmr >= _deb) {
        _flag = false;
        _tmr = millis();
        }
        return false;
        break;
      }
    }

  private:
  byte _pin;
  byte _mode;
  int _deb;
  uint32_t _tmr;
  bool _flag;

};