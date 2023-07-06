#include "ModbusRTUMaster.h"

ModbusRTUMaster::ModbusRTUMaster(HardwareSerial& serial, uint8_t dePin) {
  _hardwareSerial = &serial;
  #ifdef __AVR__
  _softwareSerial = 0;
  #endif
  #ifdef HAVE_CDCSERIAL
  _usbSerial = 0;
  #endif
  _serial = &serial;
  _dePin = dePin;
}

#ifdef __AVR__
ModbusRTUMaster::ModbusRTUMaster(SoftwareSerial& serial, uint8_t dePin) {
  _hardwareSerial = 0;
  _softwareSerial = &serial;
  #ifdef HAVE_CDCSERIAL
  _usbSerial = 0;
  #endif
  _serial = &serial;
  _dePin = dePin;
}
#endif

#ifdef HAVE_CDCSERIAL
ModbusRTUMaster::ModbusRTUMaster(Serial_& serial, uint8_t dePin) {
  _hardwareSerial = 0;
  #ifdef __AVR__
  _softwareSerial = 0;
  #endif
  _usbSerial = &serial;
  _serial = &serial;
  _dePin = dePin;
}
#endif

void ModbusRTUMaster::setTimeout(uint32_t timeout) {
  _responseTimeout = timeout;
}

void ModbusRTUMaster::begin(unsigned long baud, uint8_t config) {
  if (_hardwareSerial) {
    _calculateTimeouts(baud, config);
    _hardwareSerial->begin(baud, config);
  }
  #ifdef __AVR__
  else if (_softwareSerial) {
    _calculateTimeouts(baud, SERIAL_8N1);
    _softwareSerial->begin(baud);
  }
  #endif
  #ifdef HAVE_CDCSERIAL
  else if (_usbSerial) {
    _calculateTimeouts(baud, config);
    _usbSerial->begin(baud, config);
    while (!_usbSerial);
  }
  #endif
  if (_dePin != NO_DE_PIN) {
    pinMode(_dePin, OUTPUT);
    digitalWrite(_dePin, LOW);
  }
  _clearRxBuffer();
}



boolean ModbusRTUMaster::readCoils(uint8_t id, unsigned int startAddress, boolean *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 2000) return false;
  _buf[0] = id;
  _buf[1] = 0x01;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      uint8_t uint8_tCount = _div8RndUp(quantity);
      if (i = (3 + uint8_tCount) and _buf[1] == 0x01 and _buf[2] == uint8_tCount) {
        for (unsigned int j = 0; j < quantity; j++) {
          buf[j] = bitRead(_buf[3 + (j / 8)], j % 8);
        }
        return true;
      }
      if (i == 3 and _buf[1] == 0x81) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::readDiscreteInputs(uint8_t id, unsigned int startAddress, boolean *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 2000) return false;
  _buf[0] = id;
  _buf[1] = 0x02;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      uint8_t uint8_tCount = _div8RndUp(quantity);
      if (i = (3 + uint8_tCount) and _buf[1] == 0x02 and _buf[2] == uint8_tCount) {
        for (unsigned int j = 0; j < quantity; j++) {
          buf[j] = bitRead(_buf[3 + (j / 8)], j % 8);
        }
        return true;
      }
      if (i == 3 and _buf[1] == 0x82) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::readHoldingRegisters(uint8_t id, unsigned int startAddress, unsigned int *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 125) return false;
  _buf[0] = id;
  _buf[1] = 0x03;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      uint8_t uint8_tCount = quantity * 2;
      if (i = (3 + uint8_tCount) and _buf[1] == 0x03 and _buf[2] == uint8_tCount) {
        for (unsigned int j = 0; j < quantity; j++) {
          buf[j] = _uint8_tsToWord(_buf[3 + (j * 2)], _buf[4 + (j * 2)]);
        }
        return true;
      }
      if (i == 3 and _buf[1] == 0x83) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::readInputRegisters(uint8_t id, unsigned int startAddress, unsigned int *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 125) return false;
  _buf[0] = id;
  _buf[1] = 0x04;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      uint8_t uint8_tCount = quantity * 2;
      if (i = (3 + uint8_tCount) and _buf[1] == 0x04 and _buf[2] == uint8_tCount) {
        for (size_t j = 0; j < quantity; j++) {
          buf[j] = _uint8_tsToWord(_buf[3 + (j * 2)], _buf[4 + (j * 2)]);
        }
        return true;
      }
      if (i == 3 and _buf[1] == 0x84) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::writeSingleCoil(uint8_t id, unsigned int address, boolean value) {
  if (id ==0 or id >= 248) return false;
  _buf[0] = id;
  _buf[1] = 0x05;
  _buf[2] = highuint8_t(address);
  _buf[3] = lowuint8_t(address);
  _buf[4] = value * 0xFF;
  _buf[5] = 0;
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      if (i = 6 and _buf[1] == 0x05 and _buf[2] == highuint8_t(address) and _buf[3] == lowuint8_t(address) and _buf[4] == (value * 0xFF) and _buf[5] == 0) return true;
      if (i == 3 and _buf[1] == 0x85) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::writeSingleHoldingRegister(uint8_t id, unsigned int address, unsigned int value) {
  if (id ==0 or id >= 248) return false;
  _buf[0] = id;
  _buf[1] = 0x06;
  _buf[2] = highuint8_t(address);
  _buf[3] = lowuint8_t(address);
  _buf[4] = highuint8_t(value);
  _buf[5] = lowuint8_t(value);
  _transmit(6);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      if (i = 6 and _buf[1] == 0x06 and _buf[2] == highuint8_t(address) and _buf[3] == lowuint8_t(address) and _buf[4] == highuint8_t(value) and _buf[5] == lowuint8_t(value)) return true;
      if (i == 3 and _buf[1] == 0x86) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::writeMultipleCoils(uint8_t id, unsigned int startAddress, boolean *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 1968) return false;
  _buf[0] = id;
  _buf[1] = 0x0F;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  uint8_t uint8_tCount = _div8RndUp(quantity);
  _buf[6] = uint8_tCount;
  for (unsigned int i = 0; i < quantity; i++) {
    bitWrite(_buf[7 + (i / 8)], i % 8, buf[i]);
  }
  for (unsigned int i = quantity; i < (uint8_tCount * 8); i++) {
    bitClear(_buf[7 + (i / 8)], i % 8);
  }
  _transmit(7 + uint8_tCount);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      if (i = 6 and _buf[1] == 0x0F and _buf[2] == highuint8_t(startAddress) and _buf[3] == lowuint8_t(startAddress) and _buf[4] == highuint8_t(quantity) and _buf[5] == lowuint8_t(quantity)) return true;
      if (i == 3 and _buf[1] == 0x8F) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

boolean ModbusRTUMaster::writeMultipleHoldingRegisters(uint8_t id, unsigned int startAddress, unsigned int *buf, unsigned int quantity) {
  if (id ==0 or id >= 248 or quantity == 0 or quantity > 123) return false;
  _buf[0] = id;
  _buf[1] = 0x10;
  _buf[2] = highuint8_t(startAddress);
  _buf[3] = lowuint8_t(startAddress);
  _buf[4] = highuint8_t(quantity);
  _buf[5] = lowuint8_t(quantity);
  uint8_t uint8_tCount = quantity * 2;
  _buf[6] = uint8_tCount;
  for (unsigned int i = 0; i < quantity; i++) {
    _buf[7 + (i * 2)] = highuint8_t(_buf[i]);
    _buf[8 + (i * 2)] = lowuint8_t(_buf[i]);
  }
  _transmit(7 + uint8_tCount);
  unsigned long startTime = millis();
  while (millis() - startTime < _responseTimeout) {
    if (_serial->available() > 0) {
      size_t i = _receive();
      if (i == 0) continue;
      if (_buf[0] != id) break;
      if (i = 6 and _buf[1] == 0x10 and _buf[2] == highuint8_t(startAddress) and _buf[3] == lowuint8_t(startAddress) and _buf[4] == highuint8_t(quantity) and _buf[5] == lowuint8_t(quantity)) return true;
      if (i == 3 and _buf[1] == 0x90) _exceptionCode = _buf[2];
      break;
    }
  }
  return false;
}

uint8_t ModbusRTUMaster::getExceptionCode() {
  return _exceptionCode;
}


void ModbusRTUMaster::_setTimeouts(unsigned long baud, uint8_t config) {
  if (baud > 19200) {
    _charTimeout = 750;
    _frameTimeout = 1750;
  }
  else if (_hardwareSerial) {
    if (config == 0x2E or config == 0x3E) {
      _charTimeout = 18000000/baud;
      _frameTimeout = 42000000/baud;
    }
    else if (config == 0x0E or config == 0x26 or config == 0x36) {
      _charTimeout = 16500000/baud;
      _frameTimeout = 38500000/baud;
    }
  }
  else {
    _charTimeout = 15000000/baud;
    _frameTimeout = 35000000/baud;
  }
}

void ModbusRTUMaster::_clearRxBuf() {
  unsigned long startTime = micros();
  do {
    if (_serial->available() > 0) {
      startTime = micros();
      _serial->read();
    }
  } while (micros() - startTime < _frameTimeout);
}


void ModbusRTUMaster::_transmit(size_t len) {
  unsigned int crc = _crc(len);
  _buf[len] = lowuint8_t(crc);
  _buf[len + 1] = highuint8_t(crc);
  if (_dePin != NO_DE_PIN) digitalWrite(_dePin, HIGH);
  _serial->write(_buf, len + 2);
  _serial->flush();
  if (_dePin != NO_DE_PIN) digitalWrite(_dePin, LOW);
  _exceptionCode = 0;
}

size_t ModbusRTUMaster::_receive() {
  size_t i = 0;
  unsigned long startTime = 0;
  do {
    if (_serial->available() > 0) {
      startTime = micros();
      _buf[i] = _serial->read();
      i++;
    }
  } while (micros() - startTime < _charTimeout && i < MODBUS_RTU_MASTER_BUF_SIZE);
  while (micros() - startTime < _frameTimeout);
  if (_serial->available() == 0 and _crc(i - 2) == _uint8_tsToWord(_buf[i - 1], _buf[i - 2])) return i - 2;
  else return 0;
}

unsigned int ModbusRTUMaster::_crc(size_t len) {
  unsigned int value = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    value ^= (unsigned int)_buf[i];
    for (uint8_t j = 0; j < 8; j++) {
      boolean lsb = value & 1;
      value >>= 1;
      if (lsb == true) value ^= 0xA001;
    }
  }
  return value;
}

unsigned int ModbusRTUMaster::_div8RndUp(unsigned int value) {
  return (value + 7) >> 3;
}

unsigned int ModbusRTUMaster::_uint8_tsToWord(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}
