#ifdef TIMER_ENABLE
#define wait(x, y) while(millis() - x <= y)
#define Tupdate(x) x=millis()
#define check(x, y) if(millis() - x >= y)
#define newTimer(x) uint32_t x = millis()
#else
#define wait(x, y)
#define Tupdate(x)
#define check(x, y)
#define newTimer(x)
#endif

#ifdef LED_ENABLE
#define EnableLed(x) digitalWrite(x, 1)
#define DisableLed(x) digitalWrite(x, 0)
#define convertLed(x) digitalWrite(x, !digitalRead(x))
#define EnablePWMLed(x, y) analogWrite(x, y)
#define setLED(x) pinMode(x, OUTPUT)
#else
#define EnableLed(x)
#define DisableLed(x)
#define EnablePWMLed(x, y)
#define setLED(x)
#define convertLed(x)
#endif

#ifdef DEBUG_ENABLE
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLN(x)
#define DEBUG(x)
#endif