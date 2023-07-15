#include <ModbusRTUMaster.h>

const uint8_t rxPin = 10;
const uint8_t txPin = 11;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);
  while(!Serial);
  
  modbus.begin(38400); // modbus baud rate, config (optional)
  modbus.setTimeout(100);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    int index[4] = {0, 0, 0, 0};
    unsigned int argumentCount = 0;
    long arguments[4];
    for (byte i = 0; i < 4; i++) {
      if (i == 0) index[i] = command.indexOf(' ');
      else index[i] = command.indexOf(' ', index[i - 1]);
      if (index[i] == -1) break;
      else {
        String temp = command.substring(index[i] + 1);
        argument[i] = temp.toInt();
        argumentCount++;
      }
    }

    if (index[0] != -1) command.remove(index[0]);

    if (!command.equals("readCoil") and !command.equals("readInput") and !command.equals("readHReg") and !command.equals("readIReg") and !command.equals("writeCoil") and !command.equals("writeHReg")) Serial.println ("invalid command");
    else if ((command.startsWith("read") and argumentCount < 2) or (command.startsWith("write") and argumentCount < 3)) Serial.println("too few arguments");
    else if ((command.startsWith("read") and argumentCount > 2) or (command.startsWith("write") and argumentCount > 3)) Serial.println("too many arguments");
    else if (command.equals("readCoil")) {
      bool coils[1];
      if (!modbus.readCoils(argument[0], argument[1], coils, 1)) error();
      else Serial.println(coils[1]);
    }
    else if (command.equals("readInput")) {
      
    }
    else if (command.startsWith("readHReg")) {
      
    }
    else if (command.startsWith("readIReg")) {
      
    }
    else if (command.startsWith("write") {
      String arg1String = command.substring(index[0], index[1]);
      String arg2String = command.substring(index[1], index[2]);
      String arg3String = command.substring(index[2]);
      if (command.startsWith("writeCoil")) {
        
      }
      else if (command.startsWith("writeHReg")) {
        
      }
    }
    

    
    
    



    
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

void error() {
  if (modbus.getTimeoutFlag()) {
    Serial.println("connection timed out");
    modbus.clearTimeoutFlag();
  }
  else if (modbus.getExceptionResponse() != 0) {
    Serial.print("exception response ");
    Serial.println(modbus.getExceptionResponse());
    modbus.clearExceptionResponse();
  }
  else {
    Serial.println("an error occurred");
  }
}
