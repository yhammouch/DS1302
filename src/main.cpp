#include <Arduino.h>
#include <stdarg.h>
#include <DS1302.h>

#define MAX_CADENA  (128)

// DS1302 RTC
const int clkPin  = 6;
const int dataPin = 7;
const int cePin   = 8;

DS1302 rtc(cePin, clkPin, dataPin);

// Headers
void printSerial(const char *fmt, ...);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("#DS1302+ RTC#"));
    
    rtc.begin();

    tmElements tm;
    tm.seconds = 0;
    tm.minutes = 17;
    tm.hour = 23;
    tm.date = 2;
    tm.month = 11;
    tm.day = 6;
    tm.year = 18;

    rtc.setTime(tm);
}

void loop() {
    // put your main code here, to run repeatedly:
    Serial.println(F("Hello world!"));
    tmElements tmNow;

    tmNow = rtc.getTime();
    printSerial("Time now is: %u:%u:%u\n\r", tmNow.hour, tmNow.minutes, tmNow.seconds);
    printSerial("Date/Month/Day/Year: %u/%u/%u/%u\n\r", tmNow.date, tmNow.month, tmNow.day, tmNow.year+2000);
    delay(1000);
}

void printSerial(const char *fmt, ...) {
    char tmp[MAX_CADENA];
    va_list args;

    // Obtener la lista de argumentos
    va_start(args, fmt);

    // Escribir en tmp, con tamaño MAX_CADENA, la cadena de formato será fmt y los argumentos args
    vsnprintf(tmp, MAX_CADENA, fmt,args);
    va_end(args);

    Serial.print(tmp);
}