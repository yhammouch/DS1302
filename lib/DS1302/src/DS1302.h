#ifndef DS1302_H
#define DS1302_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#define DEBUG (0)

// RTC REGISTER ADDRESS
#define RTC_SECONDS           (0x80)
#define RTC_MINUTES           (0x82)
#define RTC_HOUR              (0x84)
#define RTC_DATE              (0x86)
#define RTC_MONTH             (0x88)
#define RTC_DAY               (0x8A)
#define RTC_YEAR              (0x8C)
#define RTC_CLOCK_BURST_WRITE (0xBE)
#define RTC_CLOCK_BURST_READ  (0xBF)


#define DELAY       (1)

// RAM

struct tmElements {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hour;
  uint8_t date;
  uint8_t month;
  uint8_t day;
  uint16_t year;
};

class DS1302 {
    public:
      DS1302(int ce_pin, int sclk_pin, int data_pin);
      void begin();
      void end();
      uint8_t read_register(uint8_t address);
      void write_register(uint8_t reg, uint8_t data);
      tmElements getTime();
      void setTime(tmElements tm);
      void _write_byte(const uint8_t data, uint8_t release);
      uint8_t _read_byte();
      uint8_t _encode(const uint16_t value);
      uint16_t _decode(const uint8_t value);
    private:
      uint8_t _ce_pin, _sclk_pin, _data_pin;
      uint8_t _decodeH(const uint8_t value);
      uint8_t _burst_read();
};

#endif