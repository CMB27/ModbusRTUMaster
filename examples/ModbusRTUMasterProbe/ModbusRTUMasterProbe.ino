/*
  ModbusRTUMasterProbe
  
  This sketch turns the Arduino into a Modbus master/client device that can be used to probe a modbus network.
  This sketch uses SoftwareSerial to communicate with the Modbus network.
  This is done so that the main hardware Serial port can be left available for USB communication on boards like the Arduino Uno and Arduino Nano.
  Because of the limitations of SoftwareSerial the serial configurations is always 8-N-1.
  Pin 10 is used as RX and pin 11 is used as TX.
  Pin 13 is configured as the DE pin.
  
  Once the sketch is loaded, open the Serial Monitor. Make sure the terminal baud rate is set to 9600.
  The prompt will guide you through the process of reading or writing data on the modbus network, or changing the Modbus baud rate.
  Basic knowledge of the Modbus protocol, the network parameters (baud rate and configuration), and the slave/server devices on the network (device addresses and data addresses) will be helpful.
  The baud rate for the Modbus network is stored in EEPROM, so if you restart or reset the arduino, it should keep this setting.
  
  Created: 2023-07-15
  By: C. M. Bulliner
  Modified: 2023-07-29
  By: C. M. Bulliner
  
*/

#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>
#include <EEPROM.h>

const uint8_t rxPin = 10;
const uint8_t txPin = 11;
const uint8_t dePin = 13;

const uint32_t baudRates[8] = {1200, 2400, 4800, 9600, 16200, 38400, 57600, 115200};
const int baudEEPROMAddress = 0;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial, dePin); // serial port, driver enable pin for rs-485 (optional)

long requestUserInput() {
  Serial.print("> ");
  while (!Serial.available()) {}
  String userInput = Serial.readStringUntil('\n');
  userInput.trim();
  Serial.println(userInput);
  return userInput.toInt();
}

long selectValue(long minValue, long maxValue) {
  long value;
  while (1) {
    value = requestUserInput();
    if (value < minValue or value > maxValue) Serial.println(F("Invalid entry, try again"));
    else return value;
  }
}

void selectBaud() {
  Serial.println(F("Select baud rate"));
  Serial.println(F("1 --- 1200"));
  Serial.println(F("2 --- 2400"));
  Serial.println(F("3 --- 4800"));
  Serial.println(F("4 --- 9600"));
  Serial.println(F("5 -- 19200"));
  Serial.println(F("6 -- 38400"));
  Serial.println(F("7 -- 57600"));
  Serial.println(F("8 - 115200"));
  uint32_t baud = baudRates[selectValue(1, 8) - 1];
  EEPROM.put(baudEEPROMAddress, baud);
  modbus.begin(baud);
  Serial.print(F("Baud rate set to "));
  Serial.println(baud);
}

void processError() {
  if (modbus.getTimeoutFlag()) {
    Serial.println(F("Connection timed out"));
    modbus.clearTimeoutFlag();
  }
  else if (modbus.getExceptionResponse() != 0) {
    Serial.print(F("Received exception response: "));
    Serial.print(modbus.getExceptionResponse());
    switch (modbus.getExceptionResponse()) {
      case 1:
        Serial.println(F(" - Illegal function"));
        break;
      case 2:
        Serial.println(F(" - Illegal data address"));
        break;
      case 3:
        Serial.println(F(" - Illegal data value"));
        break;
      case 4:
        Serial.println(F(" - Server device failure"));
        break;
      default:
        Serial.println();
        break;
    }
    modbus.clearExceptionResponse();
  }
  else {
    Serial.println("An error occurred");
  }
}

void readSingleCoil(uint8_t id, uint16_t address) {
  bool value = 0;
  if (modbus.readCoils(id, address, &value, 1)) {
    Serial.print(F("Read value of "));
    Serial.print(value);
    Serial.print(F(" from coil address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void readSingleDiscreteInput(uint8_t id, uint16_t address) {
  bool value = 0;
  if (modbus.readDiscreteInputs(id, address, &value, 1)) {
    Serial.print(F("Read value of "));
    Serial.print(value);
    Serial.print(F(" from discrete input address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void readSingleHoldingRegister(uint8_t id, uint16_t address) {
  uint16_t value = 0;
  if (modbus.readHoldingRegisters(id, address, &value, 1)) {
    Serial.print(F("Read value of "));
    Serial.print(value);
    Serial.print(F(" from holding register address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void readSingleInputRegister(uint8_t id, uint16_t address) {
  uint16_t value = 0;
  if (modbus.readInputRegisters(id, address, &value, 1)) {
    Serial.print(F("Read value of "));
    Serial.print(value);
    Serial.print(F(" from input register address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void readValue() {
  Serial.println(F("Enter device address to read from"));
  Serial.println(F("Valid entries: 1 - 247"));
  uint8_t id = selectValue(1, 247);
  
  Serial.println(F("Select datatype to read"));
  Serial.println(F("1 - Coil"));
  Serial.println(F("2 - Discrete input"));
  Serial.println(F("3 - Holding register"));
  Serial.println(F("4 - Input register"));
  uint8_t datatype = selectValue(1, 4);

  Serial.println(F("Enter data addresss"));
  Serial.println(F("Valid entries: 0 - 65535"));
  uint16_t address = selectValue(0, 65535);
  
  switch (datatype) {
    case 1:
      readSingleCoil(id, address);
      break;
    case 2:
      readSingleDiscreteInput(id, address);
      break;
    case 3:
      readSingleHoldingRegister(id, address);
      break;
    case 4:
      readSingleInputRegister(id, address);
      break;
  }
}

void writeSingleCoil(uint8_t id, uint16_t address) {
  Serial.println(F("Select value to write"));
  Serial.println(F("0 - False/off"));
  Serial.println(F("1 - True/on"));
  bool value = selectValue(0, 1);
  if (modbus.writeSingleCoil(id, address, value)) {
    Serial.print(F("Wrote value of "));
    Serial.print(value);
    Serial.print(F(" to coil address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void writeSingleHoldingRegister(uint8_t id, uint16_t address) {
  Serial.println(F("Enter value to write"));
  Serial.println(F("Valid entries: 0 - 65535"));
  uint16_t value = selectValue(0, 65535);
  if (modbus.writeSingleHoldingRegister(id, address, value)) {
    Serial.print(F("Wrote value of "));
    Serial.print(value);
    Serial.print(F(" to holding register address "));
    Serial.print(address);
    Serial.print(F(" on slave/server device "));
    Serial.println(id);
  }
  else processError();
}

void writeValue() {
  Serial.println(F("Enter device address to write to"));
  Serial.println(F("Valid entries: 0 - 247"));
  Serial.println(F("0 indicates a broadcast message"));
  uint8_t id = selectValue(0, 247);
  
  Serial.println(F("Select datatype to write"));
  Serial.println(F("1 - Coil"));
  Serial.println(F("2 - Holding register"));
  uint8_t datatype = selectValue(1, 2);

  Serial.println(F("Enter data addresss"));
  Serial.println(F("Valid entries: 0 - 65535"));
  uint16_t address = selectValue(0, 65535);
  
  switch (datatype) {
    case 1:
      writeSingleCoil(id, address);
      break;
    case 2:
      writeSingleHoldingRegister(id, address);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  while(!Serial) {}

  Serial.println(F("ModbusRTUMasterProbe"));

  uint32_t baud;
  EEPROM.get(baudEEPROMAddress, baud);
  if (baud == 0xFFFFFFFF) {
    baud = 38400;
    EEPROM.put(baudEEPROMAddress, baud);
  }
  modbus.begin(baud); // modbus baud rate, config (optional)
  modbus.setTimeout(100);
  Serial.print(F("Modbus serial port configuration: "));
  Serial.print(baud);
  Serial.println(F("-8-N-1"));
  Serial.println();
}

void loop() {
  Serial.println(F("Select an action"));
  Serial.println(F("1 - Read value"));
  Serial.println(F("2 - Write value"));
  Serial.println(F("3 - Set baud rate"));
  switch (selectValue(1, 3)) {
    case 1:
      readValue();
      break;
    case 2:
      writeValue();
      break;
    case 3:
      selectBaud();
      break;
  }
  Serial.println();
}
