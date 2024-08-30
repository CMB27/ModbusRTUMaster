#include "ModbusRTUMaster.h"

ModbusRTUMaster::ModbusRTUMaster(Stream& serial, int8_t dePin, int8_t rePin, unsigned long timeout) : _rtuComm(serial, dePin, rePin, timeout) {

}

void ModbusRTUMaster::setTimeout(unsigned long timeout) {
  _rtuComm.setTimeout(timeout);
}

void ModbusRTUMaster::begin(unsigned long baud, ConfigType config, unsigned long preDelay, unsigned long postDelay) {
  _rtuComm.begin(baud, config, preDelay, postDelay);
}



ModbusRTUMasterError ModbusRTUMaster::readCoils(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {
  return _readValues(id, 1, startAddress, buf, quantity);
}

ModbusRTUMasterError ModbusRTUMaster::readDiscreteInputs(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {
  return _readValues(id, 2, startAddress, buf, quantity);
}

ModbusRTUMasterError ModbusRTUMaster::readHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {
  return _readValues(id, 3, startAddress, buf, quantity);
}

ModbusRTUMasterError ModbusRTUMaster::readInputRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {
  return _readValues(id, 4, startAddress, buf, quantity);
}



ModbusRTUMasterError ModbusRTUMaster::writeSingleCoil(uint8_t id, uint16_t address, bool value) {
  return _writeSingleValue(id, 5, address, ((value) ? 0xFF00 : 0x0000));
}

ModbusRTUMasterError ModbusRTUMaster::writeSingleHoldingRegister(uint8_t id, uint16_t address, uint16_t value) {
  return _writeSingleValue(id, 6, address, value);
}

ModbusRTUMasterError ModbusRTUMaster::writeMultipleCoils(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {
  return _writeMultipleValues(id, 15, startAddress, buf, quantity);
}

ModbusRTUMasterError ModbusRTUMaster::writeMultipleHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {
  return _writeMultipleValues(id, 16, startAddress, buf, quantity);
}



uint8_t ModbusRTUMaster::getExceptionResponse() {
  return _exceptionResponse;
}



ModbusRTUMasterError ModbusRTUMaster::_readValues(uint8_t id, uint8_t functionCode, uint16_t startAddress, DataType buf[], uint16_t quantity) {
  if (id < 1 || id > 247) return MODBUS_RTU_MASTER_INVALID_ID;
  if (!buf) return MODBUS_RTU_MASTER_INVALID_BUFFER;
  uint16_t byteCount;
  switch (typeid(DataType)) {
    case typeid(bool):
      if (quantity == 0 || quantity > 2000) return MODBUS_RTU_MASTER_INVALID_QUANTITY;
      byteCount = _div8RndUp(quantity);
      break;
    case typeid(uint16_t):
      if (quantity == 0 || quantity > 125) return MODBUS_RTU_MASTER_INVALID_QUANTITY;
      byteCount = quantity * 2;
      break;
  }
  ModbusADU adu;
  adu.setUnitId(id);
  adu.setFunctionCode(functionCode);
  adu.setDataRegister(0, startAddress);
  adu.setDataRegister(2, quantity);
  adu.setDataLen(4);
  _rtuComm.writeAdu(adu);
  uint8_t commError = _rtuComm.readAdu(adu);
  if (commError) return _translateCommError(commError);
  if (adu.getUnitId() != id) return MODBUS_RTU_MASTER_UNEXPECTED_ID;
  if (adu.getFunctionCode() == (functionCode + 0x80)) {
    _exceptionResponse = adu.data[0];
    return MODBUS_RTU_MASTER_EXCEPTION_RESPONSE;
  }
  if (adu.getFunctionCode() != functionCode) return MODBUS_RTU_MASTER_UNEXPECTED_FUNCTION_CODE;
  if (adu.getDataLen() != (1 + byteCount)) return MODBUS_RTU_MASTER_UNEXPECTED_LENGTH;
  if (adu.data[0] != byteCount) return MODBUS_RTU_MASTER_UNEXPECTED_BYTE_COUNT;
  switch (typeid(DataType)) {
    case typeid(bool):
      for (uint16_t i = 0; i < quantity; i++) {
        buf[i] = bitRead(adu.data[1 + (i / 8)], i % 8);
      }
      break;
    case typeid(uint16_t):
      for (uint16_t i = 0; i < quantity; i++) {
        buf[i] = adu.getDataRegister(1 + (i * 2));
      }
      break;
  }
  return MODBUS_RTU_MASTER_SUCCESS;
}

ModbusRTUMasterError ModbusRTUMaster::_writeSingleValue(uint8_t id, uint8_t functionCode, uint16_t address, uint16_t value) {
  if (id > 247) return MODBUS_RTU_MASTER_INVALID_ID;
  ModbusADU adu;
  adu.setUnitId(id);
  adu.setFunctionCode(functionCode);
  adu.setDataRegister(0, address);
  adu.setDataRegister(2, value);
  adu.setDataLen(4);
  _rtuComm.writeAdu(adu);
  if (id == 0) return MODBUS_RTU_MASTER_SUCCESS;
  uint8_t commError = _rtuComm.readAdu(adu);
  if (commError) return _translateCommError(commError);
  if (adu.getUnitId() != id) return MODBUS_RTU_MASTER_UNEXPECTED_ID;
  if (adu.getFunctionCode() == (functionCode + 0x80)) {
    _exceptionResponse = adu.data[0];
    return MODBUS_RTU_MASTER_EXCEPTION_RESPONSE;
  }
  if (adu.getFunctionCode() != functionCode) return MODBUS_RTU_MASTER_UNEXPECTED_FUNCTION_CODE;
  if (adu.getDataLen() != 4) return MODBUS_RTU_MASTER_UNEXPECTED_LENGTH;
  if (adu.getDataRegister(0) != address) return MODBUS_RTU_MASTER_UNEXPECTED_ADDRESS;
  if (adu.getDataRegister(2) != value) return MODBUS_RTU_MASTER_UNEXPECTED_VALUE;
  return MODBUS_RTU_MASTER_SUCCESS;
}

ModbusRTUMasterError ModbusRTUMaster::_writeMultipleValues(uint8_t id, uint8_t functionCode, uint16_t startAddress, DataType buf[], uint16_t quantity) {
  if (id > 247) return MODBUS_RTU_MASTER_INVALID_ID;
  if (!buf) return MODBUS_RTU_MASTER_INVALID_BUFFER;
  uint16_t byteCount;
  switch (typeid(DataType)) {
    case typeid(bool):
      if (quantity == 0 || quantity > 1968) return MODBUS_RTU_MASTER_INVALID_QUANTITY;
      byteCount = _div8RndUp(quantity);
      break;
    case typeid(uint16_t):
      if (quantity == 0 || quantity > 123) return MODBUS_RTU_MASTER_INVALID_QUANTITY;
      byteCount = quantity * 2;
      break;
  }
  ModbusADU adu;
  adu.setUnitId(id);
  adu.setFunctionCode(functionCode);
  adu.setDataRegister(0, startAddress);
  adu.setDataRegister(2, quantity);
  adu.data[4] = byteCount;
  switch (typeid(DataType)) {
    case typeid(bool):
      for (uint16_t i = 0; i < quantity; i++) {
        bitWrite(adu.data[5 + (i / 8)], i % 8, buf[i]);
      }
      for (uint16_t i = quantity; i < (byteCount * 8); i++) {
        bitClear(adu.data[5 + (i / 8)], i % 8);
      }
      break;
    case typeid(uint16_t):
      for (uint16_t i = 0; i < quantity; i++) {
        adu.setDataRegister(5 + (i * 2), buf[i]);
      }
      break;
  }
  adu.setDataLen(5 + byteCount);
  if (id == 0) return MODBUS_RTU_MASTER_SUCCESS;
  uint8_t commError = _rtuComm.readAdu(adu);
  if (commError) return _translateCommError(commError);
  if (adu.getUnitId() != id) return MODBUS_RTU_MASTER_UNEXPECTED_ID;
  if (adu.getFunctionCode() == (functionCode + 0x80)) {
    _exceptionResponse = adu.data[0];
    return MODBUS_RTU_MASTER_EXCEPTION_RESPONSE;
  }
  if (adu.getFunctionCode() != functionCode) return MODBUS_RTU_MASTER_UNEXPECTED_FUNCTION_CODE;
  if (adu.getDataLen() != 4) return MODBUS_RTU_MASTER_UNEXPECTED_LENGTH;
  if (adu.getDataRegister(0) != startAddress) return MODBUS_RTU_MASTER_UNEXPECTED_ADDRESS;
  if (adu.getDataRegister(2) != quantity) return MODBUS_RTU_MASTER_UNEXPECTED_QUANTITY;
  return MODBUS_RTU_MASTER_SUCCESS;
}



ModbusRTUMasterError ModbusRTUMaster::_translateCommError(uint8_t commError) {
  switch (commError) {
    case MODBUS_RTU_COMM_SUCCESS:
      return MODBUS_RTU_MASTER_SUCCESS;
    case MODBUS_RTU_COMM_TIMEOUT:
      return MODBUS_RTU_MASTER_RESPONSE_TIMEOUT;
    case MODBUS_RTU_COMM_FRAME_ERROR:
      return MODBUS_RTU_MASTER_FRAME_ERROR;
    case MODBUS_RTU_COMM_CRC_ERROR:
      return MODBUS_RTU_MASTER_CRC_ERROR;
    default:
      return MODBUS_RTU_MASTER_UNKOWN_COMM_ERROR;
  }
}

uint16_t ModbusRTUMaster::_div8RndUp(uint16_t value) {
  return (value + 7) >> 3;
}

