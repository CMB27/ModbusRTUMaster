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

const uint8_t numCoils = 2;
const uint8_t numDiscreteInputs = 2;
const uint8_t numHoldingRegisters = 2;
const uint8_t numInputRegisters = 2;

bool coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t holdingRegisters[numHoldingRegisters];
uint16_t inputRegisters[numInputRegisters];



void printTableHeader() {
  Serial.println("| Unit ID | Function Code                         | Starting Address | Quantity | Status/Error               |");
}

void printTableSeperator() {
  Serial.println("+---------+---------------------------------------+------------------+----------+----------------------------+");
}

void printParameters(uint8_t unitId, const char* functionString, uint16_t startingAddress, uint16_t quantity) {
  char string[64];
  sprintf(string, "| %7d ", unitId);
  Serial.print(string);
  sprintf(string, "| %-37s ", functionString);
  Serial.print(string);
  sprintf(string, "| %16d ", startingAddress);
  Serial.print(string);
  sprintf(string, "| %8d ", quantity);
  Serial.print(string);
}

void printError(uint8_t error) {
  char string[64];
  if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) {
    sprintf(string, "| exception response: %02X     |\n", modbus.getExceptionResponse());
  }
  else {
    sprintf(string, "| %-26s |\n", errorString[error]);
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

  mySerial.begin(38400);
  modbus.begin(38400, SERIAL_8N1, 0, 0);
}

void loop() {
  holdingRegisters[0] = map(analogRead(knobPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(knobPins[1]), 0, 1023, 0, 255);
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);

  printTableSeperator();
  printTableHeader();
  printTableSeperator();

  uint8_t unitId = 1;
  uint16_t startingAddress[4] = {0, 0, 0, 0};

  printParameters(unitId, "16 - write multiple holding registers", startingAddress[0], numHoldingRegisters);
  printError(modbus.writeMultipleHoldingRegisters(unitId, startingAddress[0], holdingRegisters, numHoldingRegisters));

  printParameters(unitId, "15 - write multiple coils", startingAddress[1], numCoils);
  printError(modbus.writeMultipleCoils(unitId, startingAddress[1], coils, numCoils));

  printParameters(unitId, " 4 - read input registers", startingAddress[2], numInputRegisters);
  printError(modbus.readInputRegisters(unitId, startingAddress[2], inputRegisters, numInputRegisters));

  printParameters(unitId, " 2 - read discrete inputs", startingAddress[3], numDiscreteInputs);
  printError(modbus.readDiscreteInputs(unitId, startingAddress[3], discreteInputs, numDiscreteInputs));

  printTableSeperator();
  Serial.println();
  Serial.println();
  
  analogWrite(ledPins[0], inputRegisters[0]);
  analogWrite(ledPins[1], inputRegisters[1]);
  digitalWrite(ledPins[2], discreteInputs[0]);
  digitalWrite(ledPins[3], discreteInputs[1]);
}
