/*
  ModbusRTUMasterExample
  
  This example demonstrates how to setup and use the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).
  It is intended to be used with a second board running ModbusRTUSlaveExample from the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave).  
  
  Circuit:
  - The center pin of a potentiometer to pin A0, and the outside pins of the potentiometer to your board's logic level voltage (5V or 3.3V) and GND
  - The center pin of a potentiometer to pin A1, and the outside pins of the potentiometer to your board's logic level voltage (5V or 3.3V) and GND
  - A pushbutton switch from pin 2 to GND
  - A pushbutton switch from pin 3 to GND
  - A LED from pin 5 to GND with a 1K ohm series resistor
  - A LED from pin 6 to GND with a 1K ohm series resistor
  - A LED from pin 7 to GND with a 1K ohm series resistor
  - A LED from pin 8 to GND with a 1K ohm series resistor
  - RX pin (typically pin 0, pin 10 if using SoftwareSerial) to TX pin of the master/client board
  - TX pin (typically pin 1, pin 11 if using SoftwareSerial) to RX pin of the master/client board
  - GND to GND of the master/client board
  - Pin 13 is set up as the driver enable pin. This pin will be HIGH whenever the board is transmitting.

  !NOTE
  Both boards will also need power.
  
  Created: 2023-07-22
  By: C. M. Bulliner
  Last Modified: 2024-08-30
  By: C. M. Bulliner
  
*/

#include <ModbusRTUMaster.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #include <SoftwareSerial.h>
  #define USE_SOFTWARE_SERIAL
#endif

#if (defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_ARCH_MBED)) || defined(ARDUINO_NANO_ESP32)
  const int8_t buttonPins[2] = {D2, D3};
  const int8_t ledPins[4] = {D5, D6, D7, D8};
  const int8_t dePin = D13;
#else
  const int8_t buttonPins[2] = {2, 3};
  const int8_t ledPins[4] = {5, 6, 7, 8};
  const int8_t dePin = 13;
#endif
const int8_t knobPins[2] = {A0, A1};
const int8_t rePin = -1;

#if defined(USE_SOFTWARE_SERIAL)
  const int8_t rxPin = 10;
  const int8_t txPin = 11;
  SoftwareSerial mySerial(rxPin, txPin);
  #define MODBUS_SERIAL mySerial
#elif defined(ARDUINO_NANO_ESP32)
  #define MODBUS_SERIAL Serial0
#else
  #define MODBUS_SERIAL Serial1
#endif

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

const char* errorString[] = {
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
  if (error) {
    errorCounter++;
  }
  char string[128];
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) {
    sprintf(string, "%ld %ld %02X %02X %04X %04X %s: %02X\n", transactionCounter, errorCounter, unitId, functionCode, startingAddress, quantity, errorString[error], modbus.getExceptionResponse());
  }
  else {
    sprintf(string, "%ld %ld %02X %02X %04X %04X %s\n", transactionCounter, errorCounter, unitId, functionCode, startingAddress, quantity, errorString[error]);
  }
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
  
  Serial.begin(115200);

  unsigned long baud = 38400;
  uint32_t config = SERIAL_8N1;
  unsigned long preDelay = 0;
  unsigned long postDelay = 0;

  // WORKAROUND 1
  // Serial1.flush() for the following boards or board/core combination returns one byte too soon.
  // A postDelay of one byte length is calculated here. This delay is is used to keep the dePin HIGH until the full message is sent.
  #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI) || defined(ARDUINO_GIGA) || (defined(ARDUINO_NANO_RP2040_CONNECT) && defined(ARDUINO_ARCH_MBED))
    unsigned long bitsPerChar;
    switch (config) {
      case SERIAL_8E2:
      case SERIAL_8O2:
        bitsPerChar = 12;
        break;
      case SERIAL_8N2:
      case SERIAL_8E1:
      case SERIAL_8O1:
        bitsPerChar = 11;
        break;
      case SERIAL_8N1:
      default:
        bitsPerChar = 10;
        break;
    }
    postDelay = ((bitsPerChar * 1000000) / baud);
  #endif
  
  #if defined(USE_SOFTWARE_SERIAL)
    MODBUS_SERIAL.begin(baud);
    modbus.begin(baud, SERIAL_8N1, preDelay, postDelay);
  #else
    MODBUS_SERIAL.begin(baud, config);

    // WORKAROUND 2
    // The following board/core combination does not get the timings right for receiving the modbus data.
    // Telling the ModbusRTUMaster library that we are operating at a lower baud rate, causes it to loosen the timing tolerances for receiving messages.
    #if (defined(ARDUINO_NANO_RP2040_CONNECT) && defined(ARDUINO_ARCH_MBED))
      modbus.begin((baud / 4), config, preDelay, postDelay);
    #else
      modbus.begin(baud, config, preDelay, postDelay);
    #endif
    
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
