
/*
    MH-Z14A Library by Jan-Niklas Bogatka is licensed under CC BY-NC-SA 4.0.
    To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0
*/
#if !defined(MHZ14A_lib)
#define MHZ14A_lib
#endif // MH-Z14A lib

#include "Arduino.h"

#define MR_2000 0
#define MR_5000 1
#define MR_10000 2

class MHZ14A {
public:
    MHZ14A(HardwareSerial& serial);                             //Constructor only with UART
    MHZ14A(HardwareSerial& serial, HardwareSerial& logSerial);  //Constructor with UART and Hardwareserial port for debuging
#if defined(Serial_)
    MHZ14A(HardwareSerial& serial, Serial_& logSerial);         //Constructor with UART and nativ serial port for debuging (for example SerialUSB on Arduino Due)
#endif
    void begin(int timeout = 1000);                             //Method for initializing the UART and debug port
    void setPWMPin(int pin);                            //Set pin for PWM value reading
    void setAnalogPin(int pin);                            //Set pin for analog value reading
    int readConcentrationUART();                             //Function for reading the CO2-concentration in PPM
    int readConcentrationAnalog();                             //Function for reading the CO2-concentration in PPM
    int readConcentrationPWM();                             //Function for reading the CO2-concentration in PPM
    void calZeroPoint();                                    //Method to set the zero point (refere to manual)
    void calSpanPoint(uint16_t span_value_ppm = 2000);      //Method to set the zero span (refere to manual), span_value_ppm should be above 1000
    void setDetectionRange(uint8_t Range);
    void setAutoCal(bool state);
    void setDebug(bool state);                                  //Method to set the debug mode if debug port is specified
private:
    byte crc(byte msg[9]);                                      //Function to calculate the checksum
    void log(byte msg[9]);                                      //Method to send the debug messages to the serial interface
    HardwareSerial* ser;
    HardwareSerial* logSer;
    int analogPin = -1;
    int PWMPin = -1;
    int _range = 2000;
    const byte RANGE_BYTE_2000[4] = { 0x00, 0x00, 0x07, 0xD0 };
    const byte RANGE_BYTE_5000[4] = { 0x00, 0x00, 0x13, 0x88 };
    const byte RANGE_BYTE_10000[4] = { 0x00, 0x00, 0x27, 0x10 };
#if defined(Serial_)
    Serial_* logSerUSB;
#endif
    bool serIsUSB;
    bool canDebug = true;
    bool debug = false;
};
