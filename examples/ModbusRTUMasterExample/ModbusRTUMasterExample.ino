/*
  ModbusRTUMasterExample
 
  This example demonstrates how to setup and use the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).
  See README.md (https://github.com/CMB27/ModbusRTUMaster/blob/main/examples/ModbusRTUMasterExample/README.md) for hardware setup details.
  
  Created: 2023-07-22
  By: C. M. Bulliner
  Last Modified: 2024-10-27
  By: C. M. Bulliner
  
*/

#include <ModbusRTUMaster.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  // The ATmega328P and ATmega168 only have one HardwareSerial port, and on Arduino boards it is usually connected to a USB/UART bridge.
  // So, for these boards, we will use SoftwareSerial with the lbrary, leaving the HardwareSerial port available to send debugging messages.
  #define SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  const int8_t rxPin = 10;
  const int8_t txPin = 11;
  SoftwareSerial mySerial(rxPin, txPin);
  #define MODBUS_SERIAL mySerial
#elif defined(ARDUINO_NANO_ESP32)
  // On the Arduino Nano ESP32, the HardwareSerial port on pins 0 and 1 is Serial0
  #define MODBUS_SERIAL Serial0
#elif defined(ARDUINO_ARCH_STM32)
  // On ST Nucleo-64 Boards, the HardwareSerial port on pins 0 and 1 is Serial2.
  #define MODBUS_SERIAL Serial2
#else
  // On the majority of Arduino boards, the HardwareSerial port on pins 0 and 1 is Serial1
  // On the Arduino Mega and Adruino Due, Serial1 is on pins 18 and 19.
  #define MODBUS_SERIAL Serial1
#endif
// You can change the baud, config, and unit id values if you like.
// Just make sure they match the settings you use in ModbusRTUSlaveExample.
// Note, the config value will be ignored when using SoftwareSerial.
// SoftwareSerial only supports SERIAL_8N1.
#define MODBUS_BUAD 38400
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_UNIT_ID 1

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

ModbusRTUMaster modbus(MODBUS_SERIAL, dePin);

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



void setup() {
  pinMode(knobPins[0], INPUT);
  pinMode(knobPins[1], INPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);

  #if defined(ARDUINO_NANO_ESP32) || defined(ARDUINO_NANO_MATTER)
    analogReadResolution(10);
  #endif
  
  Serial.begin(115200);

  #ifndef SOFTWARE_SERIAL
    MODBUS_SERIAL.begin(MODBUS_BUAD, MODBUS_CONFIG);
    modbus.begin(MODBUS_BUAD, MODBUS_CONFIG);
  #else
    MODBUS_SERIAL.begin(MODBUS_BUAD);
    modbus.begin(MODBUS_BUAD);
  #endif
}

void loop() {
  holdingRegisters[0] = map(analogRead(knobPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(knobPins[1]), 0, 1023, 0, 255);
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);

  uint8_t error;

  error = modbus.writeMultipleHoldingRegisters(MODBUS_UNIT_ID, 0, holdingRegisters, numHoldingRegisters);
  printLog(MODBUS_UNIT_ID, 16, 0, numHoldingRegisters, error);

  error = modbus.writeMultipleCoils(MODBUS_UNIT_ID, 0, coils, numCoils);
  printLog(MODBUS_UNIT_ID, 15, 0, numHoldingRegisters, error);

  error = modbus.readInputRegisters(MODBUS_UNIT_ID, 0, inputRegisters, numInputRegisters);
  printLog(MODBUS_UNIT_ID, 4, 0, numHoldingRegisters, error);

  error = modbus.readDiscreteInputs(MODBUS_UNIT_ID, 0, discreteInputs, numDiscreteInputs);
  printLog(MODBUS_UNIT_ID, 2, 0, numHoldingRegisters, error);
  
  analogWrite(ledPins[0], inputRegisters[0]);
  analogWrite(ledPins[1], inputRegisters[1]);
  digitalWrite(ledPins[2], discreteInputs[0]);
  digitalWrite(ledPins[3], discreteInputs[1]);
}



void printLog(uint8_t unitId, uint8_t functionCode, uint16_t startingAddress, uint16_t quantity, uint8_t error) {
  transactionCounter++;
  if (error) errorCounter++;
  char string[128];
  sprintf(string, "%ld %ld %02X %02X %04X %04X %s", transactionCounter, errorCounter, unitId, functionCode, startingAddress, quantity, errorStrings[error]);
  Serial.print(string);
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) {
    sprintf(string, ": %02X", modbus.getExceptionResponse());
    Serial.print(string);
  }
  Serial.println();
}
