/*
 * ModbusRTUMasterExample
 *
 * This example demonstrates how to setup and use the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).
 * This is a communications library, so in order to test it, you will need something to communicate with.
 * A second board running ModbusRTUSlaveExample from the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave) will be needed.
 * This second board will also need to be setup with the circuit shown below.
 * 
 * This program has been tested with the following boards:
 * +-----------------------------+----------------------------------------------+-------------------------+--------+-------+--------+--------+----------------+------------+
 * | Board Name                  | Core                                         | By                      | Ver.   | Works | RX Pin | TX Pin | Port           | Post Delay |
 * +-----------------------------+----------------------------------------------+-------------------------+--------+-------+--------+--------+----------------+------------+
 * | Arduino Due                 | Arduino SAM Boards (32-bits ARM Cortex-M3)   | Arduino                 | 1.6.12 | Yes   |     19 |     18 | Serial1        |            |
 * | Arduino Giga                | Arduino Mbed OS GIGA Boards                  | Arduino                 |  4.1.5 | Yes   |      0 |      1 | Serial1        | Yes        |
 * | Arduino Leonardo            | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |      0 |      1 | Serial1        |            |
 * | Arduino Make Your UNO       | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |            |
 * | Arduino Mega 2560           | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     19 |     18 | Serial1        |            |
 * | Arduino Nano                | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |            |
 * | Arduino Nano 33 BLE         | Arduino Mbed OS Nano Boards                  | Arduino                 |  4.1.5 | Yes   |      0 |      1 | Serial1        |            |
 * | Arduino Nano 33 IoT         | Arduino SAMD Boards (32-bits ARM Cortex-M0+) | Arduino                 | 1.8.14 | Yes   |      0 |      1 | Serial1        |            |
 * | Arduino Nano ESP32          | Arduino ESP32 Boards                         | Arduino                 | 2.0.13 | Yes   |      0 |      1 | Serial0        |            |
 * | Arduino Nano ESP32          | esp32                                        | Espressif Systems       |  3.0.4 | Yes   |      0 |      1 | Serial0        |            |
 * | Arduino Nano Every          | Arduino megaAVR Boards                       | Arduino                 |  1.8.8 | Yes   |      0 |      1 | Serial1        |            |
 * | Arduino Nano Matter         | Silicon Labs                                 | Silicon Labs            |  2.1.0 | No    |      0 |      1 | Serial1        |            |
 * | Arduino Nano RP2040 Connect | Arduino Mbed OS Nano Boards                  | Arduino                 |  4.1.5 | Yes   |      0 |      1 | Serial1        | Yes        |
 * | Arduino Nano RP2040 Connect | Raspberry Pi Pico/RP2040                     | Earle F. Philhower, III |  4.0.1 | Yes   |      0 |      1 | Serial1        |            |
 * | Arduino UNO R3 SMD          | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |            |
 * | Arduino UNO R4 Minima       | Arduino UNO R4 Boards                        | Arduino                 |  1.2.0 | Yes   |      0 |      1 | Serial1        | Yes        |
 * +-----------------------------+----------------------------------------------+-------------------------+--------+-------+--------+--------+----------------+------------+
 * 
 * Note on Arduino Leonardo:
 * Closing a Serial Monitor connected to the board while running this program will cause Modbus communication errors.
 * Resetting the board should cause normal Modbus communications to resume.
 * 
 * Note on Arduino Nano ESP32:
 * When using the "esp32" core by Espressif Systems, please set pin numbering to "By GPIO number (legacy)".
 * The pin numbering option, "By Arduino pin (default)" does not appear to work correctly with this core.
 *
 * Note on Arduino Nano RP2040 Connect:
 * When using the "Arduino Mbed OS Nano Boards" core by Arduino, this board has some trouble receiving Modbus messages.
 * A workaround is implemented in this program, but it reduces the strictness of the checks on the received messages.
 *
 * Note on Arduino Nano Matter:
 * From what I can tell flush() is not properly implemented with Serial on this board.
 * ModbusRTUMaster depends on flush() to know when to set the DE and RE pins LOW.
 *
 * Circuit:
 *                                                  VCC                                                                 0.1µF Capacitor
 *                                                   ^                            Arduino Board                        /
 *                                                   |                           /                                  | |
 *  +--------------------o---------------------------o--------------------------/-------------------------------o---| |---+
 *  |                    |                                                     /                                |   | |   |
 *  |                    |         +------+                   +------+        /            RS-485               |         |
 *  |                    |      +--|      |-------------------|      |-------+        Transceiver               |         |
 *  |                    |      |  |      |                   +------+       |                    \             |         |               120 Ω Resistor
 *  |                    |      |  |      |                                  |              +-------v-------+   |         |        _____ /
 *  |                    |      |  +------+                          SCL [ ] |      +-------| RO        VCC |---+         |   +---|_____|---+
 *  |                    |      |                                    SDA [ ] |      |       |               |             |   |             |
 *  |                    |      |                                   AREF [ ] |      |   +---| RE          B |-------------|---o-------------|---<> RS485_D-
 *  |                    |      |                                    GND [ ] |      |   |   |               |             |                 |
 *  |                    |      | [ ] BOOT                            13 [ ] |------|---o---| DE          A |-------------|-----------------o---<> RS485_D+
 *  |                    +------| [ ] IOREF                           12 [ ] |      |       |               |             |
 *  |                           | [ ] RESET                          ~11 [ ] |      |   +---| DI        GND |-------------o
 *  |                           | [ ] 3.3V                           ~10 [ ] |      |   |   +---------------+             |
 *  |                           | [ ] 5V                              ~9 [ ] |      |   |                       _____     |
 *  |                           | [ ] GND                              8 [ ] |------|---|----------------->|---|_____|----o
 *  |                    +------| [ ] GND                                    |      |   |                       _____     |
 *  |                    |      | [ ] VIN                              7 [ ] |------|---|----------------->|---|_____|----o
 *  |                    |      |                                     ~6 [ ] |------|---|------------+                    |  4x LEDs
 *  |      +--------------------| [ ] A0                              ~5 [ ] |------|---|---------+  |          _____     |  4x 1K Ω Resistors
 *  |      |      +-------------| [ ] A1                               4 [ ] |      |   |         |  +---->|---|_____|----o
 *  |    __v__    |      |      | [ ] A2                              ~3 [ ] |------|---|------+  |             _____     |
 *  o---|_____|---|------o      | [ ] A3                               2 [ ] |------|---|---+  |  +------->|---|_____|----o
 *  |             |      |      | [ ] A4                            TX→1 [ ] |------|---+   |  |                          |
 *  |           __v__    |      | [ ] A5                            RX←0 [ ] |------+       |  |                          |
 *  +----------|_____|---o      |                                 __________/               |  |             __|__        |
 *                       |       \_______________________________/                          |  +-------------o   o--------o
 *        2x 10K Ω       |                                                                  |                             |  2x Pushbutton Switches
 *     Potentiometers    |                                                                  |                __|__        |
 *                       |                                                                  +----------------o   o--------o
 *                       |                                                                                                |
 *                       +------------------------------------------------------------------------------------------------o---------------------<> GND
 *
 * Components:
 * - Arduino Board
 * - Half-Duplex RS-485 Transceiver (Must be able to operate at your arduino board's logic level)
 * - 120 Ω Resistor (At least 1/4W recommended)
 * - 4x 1K Ω Resistors
 * - 2x 10K Ω Potentiometers
 * - 0.1µF Capacitor
 * - 4x LEDs
 * 
 * If using an ATmega328P or ATmega168 based board (Arduino UNO R3 and earlier, Arduino Nano, Arduino Pro Mini, etc.):
 * - Connect RO to pin 10 instead of pin 0
 * - Connect DI to pin 11 instead of pin 1
 * 
 * If using an Arduino Mega (either variant) or Arduino Due:
 * - Connect RO to pin 19 instead of pin 0
 * - Connect DI to pin 18 instead of pin 1
 * 
 * Connect the following together from this circuit and the one for ModbusRTUSlaveExample:
 * - RS485_D-
 * - RS485_D+
 * - GND
 * 
 * 
 * Created: 2023-07-22
 * By: C. M. Bulliner
 * Last Modified: 2024-09-03
 * By: C. M. Bulliner
 * 
 */

#include <ModbusRTUMaster.h>

// You can change the baud and config values if you like.
// Just make sure they match the settings you use in ModbusRTUSlaveExample.
#define MODBUS_BAUD 38400
#define MODBUS_CONFIG SERIAL_8N1

// Pre delay is a microseconds delay after the DE and RE pins go HIGH and before Serial transmission begins.
// Post delay is a microseconds delay after Serial transmission ends and before the DE and RE pins go LOW.
// Normally you would leave these at 0, but there are several boards that need a post delay in order for the library to function.
#define MODBUS_PRE_DELAY 0
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI) || defined(ARDUINO_GIGA) || (defined(ARDUINO_NANO_RP2040_CONNECT) && defined(ARDUINO_ARCH_MBED))
  // Serial1.flush() for these boards or board/core combination returns one byte too soon.
  // This messes up the operation of the DE and RE pins, causing communication errors.
  // A postDelay of one byte length is calculated here.
  #if (MODBUS_CONFIG == SERIAL_8E2) || (MODBUS_CONFIG == SERIAL_8O2)
    #define MODBUS_BITS_PER_CHAR 12
  #elif (MODBUS_CONFIG == SERIAL_8N2) || (MODBUS_CONFIG == SERIAL_8E1) || (MODBUS_CONFIG == SERIAL_8O1)
    #define MODBUS_BITS_PER_CHAR 11
  #else
    #define MODBUS_BITS_PER_CHAR 10
  #endif
  #define MODBUS_POST_DELAY ((MODBUS_BITS_PER_CHAR * 1000000) / MODBUS_BAUD)
#else
  #define MODBUS_POST_DELAY 0
#endif

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  // The ATmega328P and ATmega168 only have one HardwareSerial port, and on Arduino boards it is usually connected to a USB/UART bridge.
  // So, for these boards, we will use SoftwareSerial with the lbrary, leaving the HardwareSerial port available to send debugging messages.
  #include <SoftwareSerial.h>
  const int8_t rxPin = 10;
  const int8_t txPin = 11;
  SoftwareSerial mySerial(rxPin, txPin);
  #define MODBUS_SERIAL mySerial
#elif defined(ARDUINO_NANO_ESP32)
  // On the Arduino Nano ESP32, the HardwareSerial port on pins 0 and 1 is Serial0
  #define MODBUS_SERIAL Serial0
#else
  // On the majority of Arduino boards, the HardwareSerial port on pins 0 and 1 is Serial1
  // On the Arduino Mega and Adruino Due, Serial1 is on pins 18 and 19.
  #define MODBUS_SERIAL Serial1
#endif

#if (defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_ARCH_MBED)) || defined(ARDUINO_NANO_ESP32)
  // These boards operate unsing GPIO numbers that don't correspond to the numbers on the boards.
  // However they do have D# values #defined to correct this.
  const int8_t buttonPins[2] = {D2, D3};
  const int8_t ledPins[4] = {D5, D6, D7, D8};
  const int8_t dePin = D13;
#else
  // Other boards do not have D# values, and will throw an error if you try to use them.
  const int8_t buttonPins[2] = {2, 3};
  const int8_t ledPins[4] = {5, 6, 7, 8};
  const int8_t dePin = 13;
#endif
const int8_t knobPins[2] = {A0, A1};
const int8_t rePin = -1; // -1 means "Don't use this"

ModbusRTUMaster modbus(MODBUS_SERIAL, dePin, rePin);

const uint8_t numCoils = 2;
const uint8_t numDiscreteInputs = 2;
const uint8_t numHoldingRegisters = 2;
const uint8_t numInputRegisters = 2;

bool coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t holdingRegisters[numHoldingRegisters];
uint16_t inputRegisters[numInputRegisters];

unsigned long transactionCounter = 0;
unsigned long errorCounter = 0;

const char* errorStrings[] = {
  "success",
  "invalid id",
  "invalid buffer",
  "invalid quantity",
  "response timeout",
  "frame error",
  "crc error",
  "unknown comm error",
  "unexpected id",
  "exception response",
  "unexpected function code",
  "unexpected response length",
  "unexpected byte count",
  "unexpected address",
  "unexpected value",
  "unexpected quantity"
};



void printLog(uint8_t unitId, uint8_t functionCode, uint16_t startingAddress, uint16_t quantity, uint8_t error) {
  transactionCounter++;
  if (error) errorCounter++;
  char string[128];
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) sprintf(string, "%ld %ld %02X %02X %04X %04X %s: %02X\n", transactionCounter, errorCounter, unitId, functionCode, startingAddress, quantity, errorStrings[error], modbus.getExceptionResponse());
  else sprintf(string, "%ld %ld %02X %02X %04X %04X %s\n", transactionCounter, errorCounter, unitId, functionCode, startingAddress, quantity, errorStrings[error]);
  Serial.print(string);
}



void setup() {
  pinMode(knobPins[0], INPUT);
  pinMode(knobPins[1], INPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);

  #if defined(ARDUINO_NANO_ESP32)
    analogReadResolution(10);
  #endif
  
  Serial.begin(115200);

  // The Serial port used with ModbusRTUMaster is started separately from the library.
  // This is for broader board and processor compatibility
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    // SoftwareSerial on an AVR only supports SERIAL_8N1.
    MODBUS_SERIAL.begin(MODBUS_BAUD);
  #else
    MODBUS_SERIAL.begin(MODBUS_BAUD, MODBUS_CONFIG);
  #endif

  #if (defined(ARDUINO_NANO_RP2040_CONNECT) && defined(ARDUINO_ARCH_MBED))
    // This board/core combination does not get the timings right for receiving modbus data.
    // Telling the ModbusRTUMaster library that we are operating at a lower baud rate, causes it to loosen the timing tolerances for receiving messages.
    // This is less than ideal, but it works.
    modbus.begin(MODBUS_BAUD / 4, MODBUS_CONFIG, MODBUS_PRE_DELAY, MODBUS_POST_DELAY);
  #else
    modbus.begin(MODBUS_BAUD, MODBUS_CONFIG, MODBUS_PRE_DELAY, MODBUS_POST_DELAY);
  #endif
}

void loop() {
  holdingRegisters[0] = map(analogRead(knobPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(knobPins[1]), 0, 1023, 0, 255);
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);

  uint8_t unitId = 1;
  uint16_t startingAddress[4] = {0, 0, 0, 0};
  uint8_t error;

  error = modbus.writeMultipleHoldingRegisters(unitId, startingAddress[0], holdingRegisters, numHoldingRegisters);
  printLog(unitId, 16, startingAddress[0], numHoldingRegisters, error);

  error = modbus.writeMultipleCoils(unitId, startingAddress[1], coils, numCoils);
  printLog(unitId, 15, startingAddress[1], numHoldingRegisters, error);

  error = modbus.readInputRegisters(unitId, startingAddress[2], inputRegisters, numInputRegisters);
  printLog(unitId, 4, startingAddress[2], numHoldingRegisters, error);

  error = modbus.readDiscreteInputs(unitId, startingAddress[3], discreteInputs, numDiscreteInputs);
  printLog(unitId, 2, startingAddress[3], numHoldingRegisters, error);
  
  analogWrite(ledPins[0], inputRegisters[0]);
  analogWrite(ledPins[1], inputRegisters[1]);
  digitalWrite(ledPins[2], discreteInputs[0]);
  digitalWrite(ledPins[3], discreteInputs[1]);
}
