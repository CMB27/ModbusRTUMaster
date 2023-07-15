#include <ModbusRTUMaster.h>
#include <EEPROM.h>

const uint8_t rxPin = 10;
const uint8_t txPin = 11;

const uint32_t baudRates[8] = {1200, 2400, 4800, 9600, 16200, 38400, 57600, 115200};
const int baudEEPROMAddress = 0;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

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
    if (value < minValue or value > maxValue) Serial.println(F("Invalid value"));
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
  if (modbus.readCoils(id, address, &value, 1)) Serial.println(value);
  else processError();
}

void readSingleDiscreteInput(uint8_t id, uint16_t address) {
  bool value = 0;
  if (modbus.readDiscreteInputs(id, address, &value, 1)) Serial.println(value);
  else processError();
}

void readSingleHoldingRegister(uint8_t id, uint16_t address) {
  uint16_t value = 0;
  if (modbus.readHoldingRegisters(id, address, &value, 1)) Serial.println(value);
  else processError();
}

void readSingleInputRegister(uint8_t id, uint16_t address) {
  uint16_t value = 0;
  if (modbus.readInputRegisters(id, address, &value, 1)) Serial.println(value);
  else processError();
}

void readValue() {
  Serial.println(F("Enter device address to read from"));
  Serial.println(F("Valid values: 1 - 247"));
  uint8_t id = selectValue(1, 247);
  
  Serial.println(F("Select datatype to read"));
  Serial.println(F("1 - Coil"));
  Serial.println(F("2 - Discrete input"));
  Serial.println(F("3 - Holding register"));
  Serial.println(F("4 - Input register"));
  uint8_t datatype = selectValue(1, 4);

  Serial.println(F("Enter data addresss"));
  Serial.println(F("Valid values: 0 - 65535"));
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

void writeValue() {
  Serial.println(F("Enter device address to write to"));
  Serial.println(F("Valid values: 0 - 247"));
  Serial.println(F("0 indicates a broadcast message"));
  uint8_t id = selectValue(0, 247);
  
  Serial.println(F("Select datatype to write"));
  Serial.println(F("1 - Coil"));
  Serial.println(F("2 - Holding register"));
  uint8_t datatype = selectValue(1, 2);

  Serial.println(F("Enter data addresss"));
  Serial.println(F("Valid values: 0 - 65535"));
  uint16_t address = selectValue(0, 65535);
  
  switch (datatype) {
    case 1:
      Serial.println(F("Select select value to write"));
      Serial.println(F("0 - False"));
      Serial.println(F("1 - True"));
      if (!modbus.writeSingleCoil(id, address, (bool)selectValue(0, 1))) processError();
      break;
    case 2:
      Serial.println(F("Enter value to write"));
      Serial.println(F("Valid values: 0 - 65535"));
      if (!modbus.writeSingleHoldingRegister(id, address, (uint16_t)selectValue(0, 65535))) processError();
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  while(!Serial) {}

  uint32_t baud;
  EEPROM.get(baudEEPROMAddress, baud);
  if (baud == 0xFFFFFFFF) {
    baud = 9600;
    EEPROM.put(baudEEPROMAddress, baud);
  }
  modbus.begin(baud); // modbus baud rate, config (optional)
  modbus.setTimeout(100);
  Serial.print(F("Modbus serial port configuration: "));
  Serial.print(baud);
  Serial.println(F("-8-N-1"));
}

void loop() {
  Serial.println(F("Select action"));
  Serial.println(F("1 - Set baud rate"));
  Serial.println(F("2 - Read value"));
  Serial.println(F("3 - Write value"));
  switch (selectValue(1, 3)) {
    case 1:
      selectBaud();
      break;
    case 2:
      readValue();
      break;
    case 3:
      writeValue();
      break;
  }
}
