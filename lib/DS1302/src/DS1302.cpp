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
    delayMicroseconds(4);
}

void DS1302::_write_byte(const uint8_t data, uint8_t release) {

    for(uint8_t i = 0; i < 8; i++) {
        digitalWrite(_data_pin, (data >> i) & 0x01);
        delayMicroseconds(DELAY);
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(DELAY);

        if (release && i == 7) { // Write followed by a read
            pinMode(_data_pin, INPUT);
        } else {
            digitalWrite(_sclk_pin, LOW);
            delayMicroseconds(DELAY);
        }
    }
}


uint8_t DS1302::_read_byte() {
    uint8_t data;
    data = 0;

    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(DELAY);

        digitalWrite(_sclk_pin, LOW);
        delayMicroseconds(DELAY);
        data |= digitalRead(_data_pin) << i;
    }
    
    return data;
}


uint8_t DS1302::read_register(uint8_t address) {
    uint8_t data;

    // set lowest bit in address (read operation)
    address = address | 0x01; 
    //bitSet(address, 0);

    begin();

    _write_byte(address, true);
    data = _read_byte();

    end();

    return data;
}


void DS1302::write_register(uint8_t address, uint8_t data) {

    // clear lowest bit of address (write operation)
    //bitClear(address, 0);
    address = address & 0xFE;

    begin();

    _write_byte(address, false);
    _write_byte(data, false);

    end();
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
    rtcRegisters data;
    tmElements tm;

    _burst_read((uint8_t *) &data);

    tm.seconds  = _decode(data.secondsRegister);
    tm.minutes  = _decode(data.minutesRegister);
    tm.hour     = _decodeH(data.hourRegister);
    tm.date     = _decode(data.dateRegister);
    tm.month    = _decode(data.monthRegister);
    tm.day      = _decode(data.dayRegister);
    tm.year     = _decode(data.yearRegister);

    return tm;
}

void DS1302::_burst_read(uint8_t *p) {    
    begin();

    _write_byte(RTC_CLOCK_BURST_READ, true);

    for (uint8_t i = 0; i < 8; i++) {
        *p++ = _read_byte();
    }
    
    end();
}

void DS1302::setTime(tmElements tm) {
    begin();

    uint8_t data[8] = {0};
    data[0] = _encode(tm.seconds);
    data[1] = _encode(tm.minutes);
    data[2] = _encode(tm.hour);
    data[3] = _encode(tm.date);
    data[4] = _encode(tm.month);
    data[5] = _encode(tm.day);
    data[6] = _encode(tm.year);

    _write_byte(RTC_CLOCK_BURST_WRITE, false);

    for (uint8_t i = 0; i < 8; i++) {
        _write_byte(data[i], false);
    }
    end();
}