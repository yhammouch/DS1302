#include "DS1302.h"

DS1302::DS1302(int ce_pin, int sclk_pin, int data_pin) {
    _ce_pin   = ce_pin;
    _sclk_pin = sclk_pin;
    _data_pin = data_pin;
}

void DS1302::begin() {
    digitalWrite(_ce_pin, LOW);
    pinMode(_ce_pin, OUTPUT);

    digitalWrite(_sclk_pin, LOW);
    pinMode(_sclk_pin, OUTPUT);

    pinMode(_data_pin, OUTPUT);

    digitalWrite(_ce_pin, HIGH);
    delayMicroseconds(4);
}

void DS1302::end() {
    digitalWrite(_ce_pin, LOW);
}

void DS1302::_write_byte(const uint8_t data, uint8_t release) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        digitalWrite(_data_pin, );
    }
}

uint8_t DS1302::_read_byte() {
    uint8_t state, retValue;

    pinMode(_data_pin, INPUT);
    //digitalWrite(_sclk_pin, LOW);
    for (uint8_t i = 0; i < 8; i++) {
        state = digitalRead(_data_pin);
        retValue |= (state << i);
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(DELAY);    
        digitalWrite(_sclk_pin, LOW);
    }
    
    return retValue;
}

uint8_t DS1302::read_register(const uint8_t reg) {
    uint8_t retValue = 0;

    pinMode(_sclk_pin, OUTPUT);
    digitalWrite(_sclk_pin, LOW);
    digitalWrite(_ce_pin, HIGH);

    uint8_t command_byte = 0x81;    // read operation from rtc
    command_byte |= (reg << 1);     // add register
    _write_byte(command_byte);

    // read register
    pinMode(_data_pin, INPUT);
    uint8_t state = 0;
    for (uint8_t i = 0; i < 8; i++) {
        state = digitalRead(_data_pin);
        retValue |= (state << i);
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(DELAY);    
        digitalWrite(_sclk_pin, LOW);
    }
    digitalWrite(_ce_pin, LOW);

    if (DEBUG) {
        Serial.print(F("[RTC] Read operation->command: "));
        Serial.println(command_byte, BIN);
        Serial.print(F("[RTC] Read operation->register value: "));
        Serial.println(retValue, BIN);
        Serial.print(F("[RTC] Read operation->register value (decoded): "));
        Serial.println(_decode(retValue), DEC);
    }

    return retValue;
}

void DS1302::write_register(const uint8_t reg, const uint8_t value) {
    digitalWrite(_sclk_pin, LOW);
    digitalWrite(_ce_pin, HIGH);

    uint8_t command_byte = 0x80;    // read operation from rtc
    command_byte |= (reg << 1);     // add register

    _write_byte(command_byte);
    _write_byte(_encode(value));

    digitalWrite(_ce_pin, LOW);

    if (DEBUG) {
        Serial.print(F("[RTC] Write operation->command: "));
        Serial.println(command_byte, BIN);
        Serial.print(F("[RTC] Write operation->value: "));
        Serial.println(value, BIN);
        Serial.print(F("[RTC] Write operation->encoded value: "));
        Serial.println(_encode(value), BIN);
    }
}

uint8_t DS1302::_encode(const uint16_t value) {
    uint8_t encoded = 0;
    encoded = ((value / 10) << 4) + (value % 10);
    return encoded;
}

uint16_t DS1302::_decode(const uint8_t value) {
    uint16_t decoded = 0;
    decoded = (((value & 0xF0) >> 4)* 10) + (value & 0x0F);
    return decoded;
}

uint8_t DS1302::_decodeH(const uint8_t value) {
    uint8_t decoded;

    if (value & 0x80) {
        decoded = (value & 0x0F) + (12 * ((value & 0x20) >> 5));    
    } else {
        decoded = (value & 0x0F) + (10 * ((value & 0x30) >> 4));
    }

    return decoded;
}

tmElements DS1302::getTime() {
    tmElements tm;
    uint8_t rtc_register[8] = {0};

    for (uint8_t i = 0; i < 7; i++) {
        rtc_register[i] = read_register(i);
        delay(1);
    }

    tm.seconds  = _decode(rtc_register[0] & 0x7F);
    tm.minutes  = _decode(rtc_register[1] & 0x7F);
    tm.hour     = _decodeH(rtc_register[2] & 0xFF);
    tm.date     = _decode(rtc_register[3] & 0x3F);
    tm.month    = _decode(rtc_register[4] & 0x1F);
    tm.day      = _decode(rtc_register[5] & 0x07);
    tm.year     = _decode(rtc_register[6]);

    return tm;
}

void DS1302::setTime(tmElements tm) {
    write_register(RTC_SECONDS, tm.seconds);
    write_register(RTC_MINUTES, tm.minutes);
    write_register(RTC_HOUR,    tm.hour);
    write_register(RTC_DATE,    tm.date);
    write_register(RTC_MONTH,   tm.month);
    write_register(RTC_DAY,     tm.day);
    write_register(RTC_YEAR,    tm.year);
}

uint8_t DS1302::_burst_read() {
    begin();

    _write_byte(RTC_CLOCK_BURST_READ);


}