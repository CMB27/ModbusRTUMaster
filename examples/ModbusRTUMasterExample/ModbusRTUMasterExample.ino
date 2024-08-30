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
  - RX pin (typically pin 0 or pin 10 if using SoftwareSerial) to TX pin of the master/client board
  - TX pin (typically pin 1 or pin 11 if using SoftwareSerial) to RX pin of the master/client board
  - GND to GND of the master/client board
  - Pin 13 is set up as the driver enable pin. This pin will be HIGH whenever the board is transmitting.

  !NOTE
  Both boards will also need power.
  
  Created: 2023-07-22
  By: C. M. Bulliner
  Last Modified: 2024-08-29
  By: C. M. Bulliner
  
*/

#include <ModbusRTUMaster.h>
#include <SoftwareSerial.h>

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

const int8_t knobPins[2] = {A0, A1};
const int8_t buttonPins[2] = {2, 3};
const int8_t ledPins[4] = {5, 6, 7, 8};
const int8_t dePin = 13;
const int8_t rePin = -1;
const int8_t rxPin = 10;
const int8_t txPin = 11;

const unsigned long preDelay = 0;
const unsigned long postDelay = 0;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial, dePin, rePin);

bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

uint8_t unitId = 1;
uint16_t startingAddress = 0;
uint16_t quantity = 2;
uint8_t error;

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

  mySerial.begin(38400);
  modbus.begin(38400, SERIAL_8N1, 0, 0);
}

void loop() {
  holdingRegisters[0] = map(analogRead(knobPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(knobPins[1]), 0, 1023, 0, 255);
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);

  Serial.println("writing multiple holding registers");
  error = modbus.writeMultipleHoldingRegisters(unitId, startingAddress, holdingRegisters, quantity);
  if (error) Serial.println(errorString[error]);
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) Serial.println(modbus.getExceptionResponse(), HEX);

  Serial.println("writing multiple coils");
  error = modbus.writeMultipleCoils(1, 0, coils, 2);
  if (error) Serial.println(errorString[error]);
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) Serial.println(modbus.getExceptionResponse(), HEX);

  Serial.println("reading multiple input registers");
  error = modbus.readInputRegisters(1, 0, inputRegisters, 2);
  if (error) Serial.println(errorString[error]);
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) Serial.println(modbus.getExceptionResponse(), HEX);

  Serial.println("reading discrete inputs");
  error = modbus.readDiscreteInputs(1, 0, discreteInputs, 2);
  if (error) Serial.println(errorString[error]);
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) Serial.println(modbus.getExceptionResponse(), HEX);
  
  analogWrite(ledPins[0], inputRegisters[0]);
  analogWrite(ledPins[1], inputRegisters[1]);
  digitalWrite(ledPins[2], discreteInputs[0]);
  digitalWrite(ledPins[3], discreteInputs[1]);
}
