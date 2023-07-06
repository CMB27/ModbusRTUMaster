#ifndef ModbusRTUMaster_h
#define ModbusRTUMaster_h


#define MODBUS_RTU_MASTER_BUF_SIZE 256
#define NO_DE_PIN 255

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
    uint16_t readCoils(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    uitn16_t readDiscreteInputs(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    uitn16_t readHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);
    uitn16_t readInputRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);
    uint16_t writeSingleCoil(uint8_t id, uint16_t address, bool value);
    uint16_t writeSingleHoldingRegister(uint8_t id, uint16_t address, uint16_t value);
    uitn16_t writeMultipleCoils(uint8_t id, uint16_t startAddress, bool *buf, uint16_t quantity);
    uitn16_t writeMultipleHoldingRegisters(uint8_t id, uint16_t startAddress, uint16_t *buf, uint16_t quantity);

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
    uint32_t _responseTimeout;
    
    void __writeRequest(uint8_t len);
    bool _readResponse();
    void _clearRxBuffer();

    void _calculateTimeouts(uint32_t baud, uint8_t config);
    uint16_t _crc(uint8_t len);
    uint16_t _div8RndUp(uint16_t value);
    uint16_t _uint8_tsToWord(uint8_t high, uint8_t low);
};

#endif
