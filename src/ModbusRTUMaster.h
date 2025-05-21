#ifndef ModbusRTUMaster_h
#define ModbusRTUMaster_h

#include "Arduino.h"
#include "ModbusADU.h"
#include "ModbusRTUComm.h"
#include "ModbusMasterLogic.h"

class ModbusRTUMaster {
  public:
    ModbusRTUMaster(Stream& serial, int8_t dePin = -1, int8_t rePin = -1);
    void setTimeout(unsigned long timeout) {_rtuComm.setTimeout(timeout);}
    void begin(unsigned long baud, uint32_t config = SERIAL_8N1) {_rtuComm.begin(baud, config);}

    ModbusMasterError read(uint8_t id, uint8_t functionCode, uint16_t startAddress, bool buf[], uint16_t quantity);
    ModbusMasterError read(uint8_t id, uint8_t functionCode, uint16_t startAddress, uint16_t buf[], uint16_t quantity);

    ModbusMasterError write(uint8_t id, uint8_t functionCode, uint16_t address, bool value) {return write(id, functionCode, address, ((value) ? (uint16_t)0xFF00 : (uint16_t)0x0000));}
    ModbusMasterError write(uint8_t id, uint8_t functionCode, uint16_t address, uint16_t value);
    ModbusMasterError write(uint8_t id, uint8_t functionCode, uint16_t startAddress, bool buf[], uint16_t quantity);
    ModbusMasterError write(uint8_t id, uint8_t functionCode, uint16_t startAddress, uint16_t buf[], uint16_t quantity);

    ModbusMasterError readCoils(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {return read(id, 1, startAddress, buf, quantity);}
    ModbusMasterError readDiscreteInputs(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {return read(id, 2, startAddress, buf, quantity);}
    ModbusMasterError readHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {return read(id, 3, startAddress, buf, quantity);}
    ModbusMasterError readInputRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {return read(id, 4, startAddress, buf, quantity);}

    ModbusMasterError writeSingleCoil(uint8_t id, uint16_t address, bool value) {return write(id, 5, address, value);}
    ModbusMasterError writeSingleHoldingRegister(uint8_t id, uint16_t address, uint16_t value) {return write(id, 6, address, value);}
    ModbusMasterError writeMultipleCoils(uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity) {return write(id, 15, startAddress, buf, quantity);}
    ModbusMasterError writeMultipleHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity) {return write(id, 16, startAddress, buf, quantity);}

    uint8_t getExceptionResponse() {return ModbusMasterLogic.getExceptionResponse();}
    ModbusRTUCommError getCommError() {return _commError;}

  private:
    ModbusRTUComm _rtuComm;
    ModbusRTUCommError _commError = MODBUS_RTU_COMM_SUCCESS;

};

#endif
