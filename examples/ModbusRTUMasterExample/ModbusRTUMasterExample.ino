#include <ModbusRTUMaster.h>

const byte rxPin = 10;
const byte txPin = 11;
const byte slaveId = 1;

const unsigned int numCoils = 4;
const unsigned int numDiscreteInputs = 8;
const unsigned int numHoldingRegisters = 2;
const unsigned int numInputRegisters = 2;

bool coilsIn[numCoils];
bool coilsOut[numCoils];
bool discreteInputs[numDiscreteInputs];
unsigned int holdingRegistersIn[numHoldingRegisters];
unsigned int holdingRegistersOut[numHoldingRegisters];
unsigned int inputRegisters[numInputRegisters];

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

void setup() {
  modbus.setTimeout(100);
  modbus.begin(38400); // modbus baud rate, config (optional)
}

void loop() {
  modbus.readCoils(slaveId, 0x2000, coilsIn, 4); // slave id/address, starting data address, buffer, number of coils to read
  modbus.readDiscreteInputs(slaveId, 0x0020, discreteInputs, 8); // slave id/address, starting data address, buffer, number of discrete inputs to read
  modbus.readHoldingRegisters(slaveId, 0x0000, holdingRegistersIn, 2); // slave id/address, starting data address, buffer, number of holding registers to read
  modbus.readInputRegisters(slaveId, 0xE004, inputRegisters, 2); // slave id/address, starting data address, buffer, number of input registers to read

  // add way of manipulating arrays here

  modbus.writeMultipleCoils(slaveId, 0x2000, coilsOut, 4); // slave id/address, starting data address, buffer, number of coils to write
  modbus.writeMultipleHoldingRegisters(slaveId, 0x0000, holdingRegistersOut, 2); // slave id/address, starting data address, buffer, number of holding registers to write
}
