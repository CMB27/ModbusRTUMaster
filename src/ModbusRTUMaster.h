#ifndef ModbusRTUMaster_h
#define ModbusRTUMaster_h


#define MODBUS_RTU_MASTER_BUF_SIZE 256
#define NO_DE_PIN 255

/*
#define MODBUS_READ_COILS_FUNCTION_CODE 1
#define MODBUS_READ_DISCRETE_INPUTS_FUNCTION_CODE 2
#define MODBUS_READ_HOLDING_REGISTERS_FUNCTION_CODE 3
#define MODBUS_READ_INPUPT_REGISTERS_FUNCTION_CODE 4
#define MODBUS_WRITE_SINGLE_COIL_FUNCTION_CODE 5
#define MODBUS_WRITE_SINGLE_HOLDING_REGISTER_FUNCTION_CODE 6
#define MODBUS_WRITE_MULTIPLE_COILS_FUNCTION CODE 15
#define MODBUS_WRITE_MULTIPLE_HOLDING_REGISTERS_FUNCTION_CODE 16
*/

#include "Arduino.h"
#ifdef __AVR__
#include <SoftwareSerial.h>
#endif

class ModbusRTUMaster {
  public:
    ModbusRTUMaster(HardwareSerial& serial, uint8_t dePin = NO_DE_PIN);
    #ifdef __AVR__
    ModbusRTUMaster(SoftwareSerial& serial, uint8_t dePin = NO_DE_PIN);
    #endif
    #ifdef HAVE_CDCSERIAL
    ModbusRTUMaster(Serial_& serial, uint8_t dePin = NO_DE_PIN);
    #endif
    void setTimeout(uint32_t timeout);
    void begin(uint32_t baud, uint8_t config = SERIAL_8N1);
    bool readCoils(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    bool readDiscreteInputs(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    bool readHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);
    bool readInputRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);
    bool writeSingleCoil(uint8_t id, uint16_t address, bool value);
    bool writeSingleHoldingRegister(uint8_t id, uint16_t address, uint16_t value);
    bool writeMultipleCoils(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    bool writeMultipleHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);
    bool getTimeoutFlag();
    void clearTimeoutFlag();
    uint8_t getExceptionResponse();
    void clearExceptionResponse();

  private:
    HardwareSerial *_hardwareSerial;
    #ifdef __AVR__
    SoftwareSerial *_softwareSerial;
    #endif
    #ifdef HAVE_CDCSERIAL
    Serial_ *_usbSerial;
    #endif
    Stream *_serial;
    uint8_t _dePin;
    uint8_t _buf[MODBUS_RTU_MASTER_BUF_SIZE];
    uint32_t _charTimeout;
    uint32_t _frameTimeout;
    uint32_t _responseTimeout = 100;
    bool _timeoutFlag = false;
    uint8_t _exceptionResponse = 0;
    
    void __writeRequest(uint8_t len);
    uint16_t _readResponse(uint8_t id, uint8_t function);
    void _clearRxBuffer();

    void _calculateTimeouts(uint32_t baud, uint8_t config);
    uint16_t _crc(uint8_t len);
    uint16_t _div8RndUp(uint16_t value);
    uint16_t _bytesToWord(uint8_t high, uint8_t low);
};

#endif
