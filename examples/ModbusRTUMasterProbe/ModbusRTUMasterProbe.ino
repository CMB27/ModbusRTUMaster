/*
  ModbusRTUMasterProbe
  
  This sketch turns the Arduino into a Modbus master/client device that can be used to probe a modbus network.
  See README.md (https://github.com/CMB27/ModbusRTUMaster/blob/main/examples/ModbusRTUMasterProbe/README.md) for hardware setup details.
  
  Once the sketch is loaded, open the Serial Monitor and set the terminal baud rate to 115200.
  The prompt will guide you through the process of reading or writing data on the modbus network, or changing the Modbus baud rate and configuration.
  Basic knowledge of the Modbus protocol, the network parameters (baud rate and configuration), and the slave/server devices on the network (device addresses and data addresses) will be helpful.
  
  Created: 2023-07-15
  By: C. M. Bulliner
  Modified: 2024-09-07
  By: C. M. Bulliner
  
*/

#include <ModbusRTUMaster.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #define SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  const int8_t rxPin = 10;
  const int8_t txPin = 11;
  SoftwareSerial mySerial(rxPin, txPin);
  #define MODBUS_SERIAL mySerial
#elif defined(ARDUINO_NANO_ESP32)
  #define MODBUS_SERIAL Serial0
#else
  #define MODBUS_SERIAL Serial1
#endif

#if (defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_ARCH_MBED)) || defined(ARDUINO_NANO_ESP32)
  const int8_t dePin = D13;
#else
  const int8_t dePin = 13;
#endif

ModbusRTUMaster modbus(MODBUS_SERIAL, dePin);

const uint8_t numBaudRates = 6;
const unsigned long baudRates[numBaudRates] = {4800, 9600, 19200, 38400, 57600, 115200};

const uint8_t numConfigs = 6;
const uint32_t configs[numConfigs] = {SERIAL_8N1, SERIAL_8N2, SERIAL_8E1, SERIAL_8E2, SERIAL_8O1, SERIAL_8O2};
const char* configStrings[numConfigs] = {"8-N-1", "8-N-2", "8-E-1", "8-E-2", "8-O-1", "8-O-2"};

const uint8_t numMainMenuOptions = 3;
const char* mainMenuOptions[numMainMenuOptions] = {"Read value", "Write value", "Set serial configuration"};

const uint8_t numDatatypes = 4;
const char* datatypeStrings[numDatatypes] {"Coil", "Discrete input", "Holding register", "Input register"};

const uint8_t numErrorStrings = 12;
const char* errorStrings[numErrorStrings] = {
  "Response timeout",
  "Frame error in response",
  "CRC error in response",
  "Unknown communication error",
  "Unexpected unit ID in response",
  "Exception response ",
  "Unexpected function code in response",
  "Unexpected response length",
  "Unexpected byte count in response",
  "Unexpected data address in response",
  "Unexpected value in response",
  "Unexpected quantity in response"
};

const uint8_t numExceptionResponseStrings = 4;
const char* exceptionResponseStrings[] = {"illegal function", "illegal data address", "illegal data value", "server device failure"};

long requestUserInput() {
  Serial.print("> ");
  while (!Serial.available());
  String userInput = Serial.readStringUntil('\n');
  userInput.trim();
  Serial.println(userInput);
  return userInput.toInt();
}

long selectValue(long minValue, long maxValue) {
  long value;
  while (1) {
    value = requestUserInput();
    if (value < minValue || value > maxValue) Serial.println(F("Invalid entry, try again"));
    else return value;
  }
}

void printSerialConfig(uint8_t baudSelection, uint8_t configSelection) {
  Serial.print("Serial configuration set to ");
  Serial.print(baudRates[baudSelection]);
  Serial.print("-");
  Serial.print(configStrings[configSelection]);
  Serial.println();
}

void printMenu(const char* menuStrings[], uint8_t numMenuOptions) {
  for (uint8_t i = 0; i < numMenuOptions; i++) {
    Serial.print(i + 1);
    Serial.print(" - ");
    Serial.println(menuStrings[i]);
  }
}

void setSerialConfig() {
  Serial.println("Select baud rate");
  for (uint8_t i = 0; i < numBaudRates; i++) {
    Serial.print(i + 1);
    Serial.print(" -");
    if (baudRates[i] < 100000) Serial.print("-");
    if (baudRates[i] < 10000) Serial.print("-");
    Serial.print(" ");
    Serial.println(baudRates[i]);
  }
  uint8_t baudSelection = selectValue(1, numBaudRates + 1) - 1;
  uint8_t configSelection = 0;
  #ifndef SOFTWARE_SERIAL
    Serial.println("Select configuration");
    printMenu(configStrings, numConfigs);
    configSelection = selectValue(1, numConfigs + 1) - 1;
    MODBUS_SERIAL.begin(baudRates[baudSelection], configs[configSelection]);
    modbus.begin(baudRates[baudSelection], configs[configSelection]);
  #else
    MODBUS_SERIAL.begin(baudRates[baudSelection]);
    modbus.begin(baudRates[baudSelection]);
  #endif
  printSerialConfig(baudSelection, configSelection);
}

void printError(uint8_t error) {
  if (!error) return;
  Serial.print("Error: ");
  if (error >= 4 && error < (numErrorStrings + 4)) {
    Serial.print(errorStrings[error - 4]);
    if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) {
      uint8_t exceptionResponse = modbus.getExceptionResponse();
      Serial.print(exceptionResponse);
      if (exceptionResponse >= 1 && exceptionResponse <= numExceptionResponseStrings) {
        Serial.print(" - ");
        Serial.print(exceptionResponseStrings[exceptionResponse - 1]);
      }
    }
  }
  else Serial.print("Unknown error");
  Serial.println();
}

void readValue() {
  Serial.println("Enter a unit ID to read from");
  Serial.println("Valid entries: 1 - 247");
  uint8_t unitId = selectValue(1, 247);
  Serial.println("Select datatype to read");
  printMenu(datatypeStrings, numDatatypes);
  uint8_t datatype = selectValue(1, 4);
  Serial.println("Enter data addresss");
  Serial.println("Valid entries: 0 - 65535");
  uint16_t address = selectValue(0, 65535);
  uint16_t value = 0;
  uint8_t error = 0;
  switch (datatype) {
    case 1:
      error = modbus.readCoils(unitId, address, (bool*)&value, 1);
      break;
    case 2:
      error = modbus.readDiscreteInputs(unitId, address, (bool*)&value, 1);
      break;
    case 3:
      error = modbus.readHoldingRegisters(unitId, address, &value, 1);
      break;
    case 4:
      error = modbus.readInputRegisters(unitId, address, &value, 1);
      break;
  }
  if (!error) {
    String datatypeString = datatypeStrings[datatype - 1];
    datatypeString.toLowerCase();
    Serial.print("Read value of ");
    Serial.print(value);
    Serial.print(" from ");
    Serial.print(datatypeString);
    Serial.print(" address ");
    Serial.print(address);
    Serial.print(" on slave/server device ");
    Serial.println(unitId);
  }
  else printError(error);
}

void writeValue() {
  Serial.println("Enter a unit ID to write to");
  Serial.println("Valid entries: 0 - 247");
  Serial.println("0 indicates a broadcast message");
  uint8_t unitId = selectValue(0, 247);
  Serial.println("Select datatype to write");
  const char* writableDatatypeStrings[2] = {datatypeStrings[0], datatypeStrings[2]};
  printMenu(writableDatatypeStrings, 2);
  uint8_t datatype = selectValue(1, 2);
  Serial.println("Enter data addresss");
  Serial.println("Valid entries: 0 - 65535");
  uint16_t address = selectValue(0, 65535);
  uint16_t value = 0;
  uint8_t error = 0;
  switch (datatype) {
    case 1:
      Serial.println("Select value to write");
      Serial.println("0 - False/off");
      Serial.println("1 - True/on");
      value = selectValue(0, 1);
      error = modbus.writeSingleCoil(unitId, address, (bool)value);
      break;
    case 2:
      Serial.println("Enter value to write");
      Serial.println("Valid entries: 0 - 65535");
      value = selectValue(0, 65535);
      error = modbus.writeSingleHoldingRegister(unitId, address, value);
      break;
  }
  if (!error) {
    String datatypeString = datatypeStrings[datatype - 1];
    datatypeString.toLowerCase();
    Serial.print("Wrote value of ");
    Serial.print(value);
    Serial.print(" to ");
    Serial.print(datatypeString);
    Serial.print(" address ");
    Serial.print(address);
    Serial.print(" on slave/server device ");
    Serial.println(unitId);
  }
  else printError(error);
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  while(!Serial);
  Serial.println("\n\nModbusRTUMasterProbe");
  setSerialConfig();
  Serial.println();
}

void loop() {
  Serial.println(F("Select an action"));
  printMenu(mainMenuOptions, numMainMenuOptions);
  switch (selectValue(1, numMainMenuOptions)) {
    case 1:
      readValue();
      break;
    case 2:
      writeValue();
      break;
    case 3:
      setSerialConfig();
      break;
  }
  Serial.println();
}
