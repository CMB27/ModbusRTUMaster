#include <ModbusRTUMaster.h>

const uint8_t rxPin = 10;
const uint8_t txPin = 11;
const uint8_t slaveId = 1;

const uint16_t numCoils = 4;
const uint16_t numDiscreteInputs = 4;
const uint16_t numHoldingRegisters = 4;
const uint16_t numInputRegisters = 1;

bool coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t holdingRegisters[numHoldingRegisters];
uint16_t inputRegisters[numInputRegisters];

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  while(!Serial);
  
  modbus.begin(38400); // modbus baud rate, config (optional)
  modbus.setTimeout(100);
}

void loop() {
  if (Serial.available()) {
    String topic = Serial.readStringUntil('\n');
    topic.trim();
    int index = topic.indexOf(' ');
    if (index != -1) {
      String payload = topic.substring(index + 1);
      topic.remove(index);
      if      (topic == "coil1") modbus.writeSingleCoil(slaveId, 0x2000, (bool)payload.toInt());
      else if (topic == "coil2") modbus.writeSingleCoil(slaveId, 0x2001, (bool)payload.toInt());
      else if (topic == "coil3") modbus.writeSingleCoil(slaveId, 0x2002, (bool)payload.toInt());
      else if (topic == "coil4") modbus.writeSingleCoil(slaveId, 0x2003, (bool)payload.toInt());
      else if (topic == "hreg1") modbus.writeSingleHoldingRegister(slaveId, 0x0000, (uint16_t)payload.toInt());
      else if (topic == "hreg2") modbus.writeSingleHoldingRegister(slaveId, 0x0001, (uint16_t)payload.toInt());
      else if (topic == "hreg3") modbus.writeSingleHoldingRegister(slaveId, 0x0002, (uint16_t)payload.toInt());
      else if (topic == "hreg4") modbus.writeSingleHoldingRegister(slaveId, 0x0003, (uint16_t)payload.toInt());
    }
    
    modbus.readCoils(slaveId, 0x2000, coils, numCoils); // slave id/address, starting data address, buffer, number of coils to read
    modbus.readDiscreteInputs(slaveId, 0x0000, discreteInputs, numDiscreteInputs); // slave id/address, starting data address, buffer, number of discrete inputs to read
    modbus.readHoldingRegisters(slaveId, 0x0000, holdingRegisters, numHoldingRegisters); // slave id/address, starting data address, buffer, number of holding registers to read
    modbus.readInputRegisters(slaveId, 0xE000, inputRegisters, numInputRegisters); // slave id/address, starting data address, buffer, number of input registers to read
    
    Serial.print(topic);
    Serial.print(' ');
    if (topic == "?") {
      Serial.println();
      Serial.println(F("ModbusRTUMasterExample"));
      Serial.println();
      Serial.println(F("| TOPIC  | R/W | RANGE      |"));
      Serial.println(F("|--------|-----|------------|"));
      Serial.println(F("| coil1  | R/W | 0 or 1     |"));
      Serial.println(F("| coil2  | R/W | 0 or 1     |"));
      Serial.println(F("| coil3  | R/W | 0 or 1     |"));
      Serial.println(F("| coil4  | R/W | 0 or 1     |"));
      Serial.println(F("| input1 | R   | 0 or 1     |"));
      Serial.println(F("| input2 | R   | 0 or 1     |"));
      Serial.println(F("| input3 | R   | 0 or 1     |"));
      Serial.println(F("| input4 | R   | 0 or 1     |"));
      Serial.println(F("| hreg1  | R/W | 0 to 65535 |"));
      Serial.println(F("| hreg2  | R/W | 0 to 65535 |"));
      Serial.println(F("| hreg3  | R/W | 0 to 65535 |"));
      Serial.println(F("| hreg4  | R/W | 0 to 65535 |"));
      Serial.println(F("| inreg1 | R   | 0 to 65535 |"));
      Serial.println();
    }
    else if (topic == "coil1")  Serial.print(coils[0]);
    else if (topic == "coil2")  Serial.print(coils[1]);
    else if (topic == "coil3")  Serial.print(coils[2]);
    else if (topic == "coil4")  Serial.print(coils[3]);
    else if (topic == "input1") Serial.print(discreteInputs[0]);
    else if (topic == "input2") Serial.print(discreteInputs[1]);
    else if (topic == "input3") Serial.print(discreteInputs[2]);
    else if (topic == "input4") Serial.print(discreteInputs[3]);
    else if (topic == "hreg1")  Serial.print(holdingRegisters[0]);
    else if (topic == "hreg2")  Serial.print(holdingRegisters[1]);
    else if (topic == "hreg3")  Serial.print(holdingRegisters[2]);
    else if (topic == "hreg4")  Serial.print(holdingRegisters[3]);
    else if (topic == "inreg1") Serial.print(inputRegisters[0]);
    Serial.println();
  }
}
