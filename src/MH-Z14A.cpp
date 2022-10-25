
/*
    MH-Z14A Library by Jan-Niklas Bogatka is licensed under CC BY-NC-SA 4.0.
    To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0
*/

#include "MH-Z14A.h"

MHZ14A::MHZ14A(HardwareSerial& serial) {
    ser = &serial;
    canDebug = false;
}

MHZ14A::MHZ14A(HardwareSerial& serial, HardwareSerial& logSerial) {
    ser = &serial;
    serIsUSB = false;
    logSer = &logSerial;
}

#if defined(Serial_)
MHZ14A::MHZ14A(HardwareSerial& serial, Serial_& logSerial) {
    ser = &serial;
    serIsUSB = true;
    logSerUSB = &logSerial;
}
#endif

void MHZ14A::begin(int timeout) {
    ser->begin(9600);
    ser->setTimeout(timeout);
}

void MHZ14A::setAnalogPin(int pin) {
    analogPin = pin;
}

void MHZ14A::setPWMPin(int pin) {
    PWMPin = pin;
}

void MHZ14A::setDebug(bool state) {
    if (canDebug) debug = state;
    else debug = false;
}

void MHZ14A::log(byte msg[9]) {
    if (debug) {
        if (!serIsUSB) for (int i = 0; i < 9; i++) logSer->print(msg[i], HEX);
        else {
#if defined(Serial_)
            for (int i = 0; i < 9; i++) logSerUSB->print(msg[i], HEX);
#endif
        }
        if (!serIsUSB) logSer->println("");
        else {
#if defined(Serial_)
            logSerUSB->println("");
#endif
        }
    }
}

byte MHZ14A::crc(byte msg[9]) {
    byte chksum = 0;
    for (int i = 1; i < 8; i++) {
        chksum += msg[i];
    }
    chksum = 0xff - chksum;
    chksum += 1;
    return chksum;
}

int MHZ14A::readConcentrationUART() {
    byte msg[] = { 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    byte msgr[] = { 0,0,0,0,0,0,0,0,0 };
    int concentrationPPM;
    msg[8] = crc(msg);
    log(msg);
    ser->write(msg, 9);
    int timeout = 60;
    ser->readBytes(msgr, 9);
    log(msgr);
    if (msgr[0] != 0xFF) return -1;
    if (crc(msgr) == msgr[8]) {
        concentrationPPM = int(msgr[2]) * 256 + int(msgr[3]);
    }
    else {
        concentrationPPM = -1;
    }

    return concentrationPPM;
}

int MHZ14A::readConcentrationAnalog() {
    if (analogPin == -1) return -1;

    float voltage = analogRead(analogPin) * 3.3 / 4095.0;
    int gas_concentration = int((voltage - 0.4) * (_range / 1.6));

    return gas_concentration > 0 ? gas_concentration : 0;
}

int MHZ14A::readConcentrationPWM() {
    if (PWMPin == -1) return -1;

    unsigned long th = pulseIn(PWMPin, HIGH, 2 * 1000UL * 1000UL);
    unsigned long tl = pulseIn(PWMPin, LOW, 2 * 1000UL * 1000UL);
    long ppm = _range * (th - 2) / (th + tl - 4);

    return int(ppm);
}

void MHZ14A::calZeroPoint() {
    byte msg[9] = { 0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    msg[8] = crc(msg);
    ser->write(msg, 9);
}

void MHZ14A::calSpanPoint(uint16_t span_value_ppm) {
    byte span_high = span_value_ppm / 256;
    byte span_low = span_value_ppm % 256;
    byte msg[9] = { 0xFF, 0x01, 0x88, span_high, span_low, 0x00, 0x00, 0x00, 0x00 };
    msg[8] = crc(msg);
    ser->write(msg, 9);
}

void MHZ14A::setDetectionRange(uint8_t Range) {
    const byte* _range_byte;
    switch (Range) {
    case 0:
        _range_byte = RANGE_BYTE_2000;
        _range = 2000;
        break;
    case 1:
        _range_byte = RANGE_BYTE_5000;
        _range = 5000;
        break;
    case 2:
        _range_byte = RANGE_BYTE_10000;
        _range = 10000;
        break;
    }

    byte msg[9] = { 0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (int i = 4; i < 8; i++) {
        msg[i] = _range_byte[i - 4];
    }
    msg[8] = crc(msg);
    ser->write(msg, 9);
}

void MHZ14A::setAutoCal(bool state) {
    byte msg[9] = { 0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (state) {
        msg[3] = 0xA0;
    }
    else {
        msg[3] = 0x00;
    }
    msg[8] = crc(msg);
    ser->write(msg, 9);
}