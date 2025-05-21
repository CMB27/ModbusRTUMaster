#include "ModbusRTUMaster.h"

ModbusRTUMaster::ModbusRTUMaster(Stream& serial, int8_t dePin, int8_t rePin) : _rtuComm(serial, dePin, rePin) {
  _rtuComm.setTimeout(500);
}



ModbusMasterError ModbusRTUMaster::read(uint8_t id, uint8_t functionCode, uint16_t startAddress, bool buf[], uint16_t quantity) {
  if (id < 1 || id > 247) return MODBUS_MASTER_INVALID_ID;
  ModbusADU adu;
  ModbusMasterError logicError = ModbusMasterLogic.prepareRequest(adu, id, functionCode, startAddress, quantity);
  if (logicError) return logicError;
  _rtuComm.writeAdu(adu);
  _commError = _rtuComm.readAdu(adu);
  if (_commError) return MODBUS_MASTER_COMM_ERROR;
  return ModbusMasterLogic.processResponse(adu, id, functionCode, startAddress, buf, quantity);
}

ModbusMasterError ModbusRTUMaster::read(uint8_t id, uint8_t functionCode, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {
  if (id < 1 || id > 247) return MODBUS_MASTER_INVALID_ID;
  ModbusADU adu;
  ModbusMasterError logicError = ModbusMasterLogic.prepareRequest(adu, id, functionCode, startAddress, quantity);
  if (logicError) return logicError;
  _rtuComm.writeAdu(adu);
  _commError = _rtuComm.readAdu(adu);
  if (_commError) return MODBUS_MASTER_COMM_ERROR;
  return ModbusMasterLogic.processResponse(adu, id, functionCode, startAddress, buf, quantity);
}



ModbusMasterError ModbusRTUMaster::write(uint8_t id, uint8_t functionCode, uint16_t address, uint16_t value) {
  if (id > 247) return MODBUS_MASTER_INVALID_ID;
  ModbusADU adu;
  ModbusMasterError logicError = ModbusMasterLogic.prepareRequest(adu, id, functionCode, address, value);
  if (logicError) return logicError;
  _rtuComm.writeAdu(adu);
  if (id == 0) return MODBUS_RTU_MASTER_SUCCESS;
  _commError = _rtuComm.readAdu(adu);
  if (_commError) return MODBUS_MASTER_COMM_ERROR;
  return ModbusMasterLogic.processResponse(adu, id, functionCode, address, value);
}

ModbusMasterError ModbusRTUMaster::write(uint8_t id, uint8_t functionCode, uint16_t startAddress, bool buf[], uint16_t quantity) {
  if (id > 247) return MODBUS_RTU_MASTER_INVALID_ID;
  ModbusADU adu;
  ModbusMasterError logicError = ModbusMasterLogic.prepareRequest(adu, id, functionCode, startAddress, buf, quantity);
  if (logicError) return logicError;
  _rtuComm.writeAdu(adu);
  if (id == 0) return MODBUS_RTU_MASTER_SUCCESS;
  _commError = _rtuComm.readAdu(adu);
  if (_commError) return MODBUS_MASTER_COMM_ERROR;
  return ModbusMasterLogic.processResponse(adu, id, functionCode, startAddress, quantity);
}

ModbusMasterError ModbusRTUMaster::write(uint8_t id, uint8_t functionCode, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {
  if (id > 247) return MODBUS_RTU_MASTER_INVALID_ID;
  ModbusADU adu;
  ModbusMasterError logicError = ModbusMasterLogic.prepareRequest(adu, id, functionCode, startAddress, buf, quantity);
  if (logicError) return logicError;
  _rtuComm.writeAdu(adu);
  if (id == 0) return MODBUS_RTU_MASTER_SUCCESS;
  _commError = _rtuComm.readAdu(adu);
  if (_commError) return MODBUS_MASTER_COMM_ERROR;
  return ModbusMasterLogic.processResponse(adu, id, functionCode, startAddress, quantity);
}