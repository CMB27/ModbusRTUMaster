# ModbusRTUMaster
Modbus is an industrial communication protocol. The RTU variant communicates over serial lines such as UART, RS-232, or RS-485. The full details of the Modbus protocol can be found at [modbus.org](https://modbus.org). A good summary can also be found on [Wikipedia](https://en.wikipedia.org/wiki/Modbus).  

This is an Arduino library that implements the master/client logic of the Modbus RTU protocol. This library implements function codes 1 (Read Coils), 2 (Read Discrete Inputs), 3 (Read Holding Registers), 4 (Read Input Registers), 5 (Write Single Coil), 6 (Write Single Holding Register), 15 (Write Multiple Coils), and 16 (Write Multiple Holding Registers).  

This library will work with HardwareSerial, SoftwareSerial, or Serial_ (USB Serial on ATmega32u4 based boards). A driver enable pin can be set up, enabling an RS-485 transceiver to be used. Only `SERIAL_8N1`, `SERIAL_8E1`, `SERIAL_8O1`, `SERIAL_8N2`, `SERIAL_8E2`, and `SERIAL_8O2` are supported when using HardwareSerial; attempting to use any other configuration will cause the library to default to `SERIAL_8N1`.  

## Examples
- [ModbusRTUMasterExample](https://github.com/CMB27/ModbusRTUMaster/blob/main/extras/ModbusRTUMasterExample.md)
- [ModbusRTUMasterProbe](https://github.com/CMB27/ModbusRTUMaster/blob/main/extras/ModbusRTUMasterProbe.md)

## Methods

### ModbusRTUMaster()

#### Description
Creates a ModbusRTUMaster object and sets the serial port to use for data transmission.
Optionally sets a driver enable pin. This pin will go `HIGH` when the library is transmitting. This is primarily intended for use with an RS-485 transceiver, but it can also be a handy diagnostic when connected to an LED.

#### Syntax
``` C++
ModbusRTUMaster(serial)
ModbusRTUMaster(serial, dePin)
```

#### Parameters
- `serial`: the serial port object to use for Modbus communication.
- `dePin`: the driver enable pin. This pin is set HIGH when transmitting. If this parameter is set to `NO_DE_PIN`, this feature will be disabled. Default value is `NO_DE_PIN`. Allowed data types `uint8_t` or `byte`.

#### Example
``` C++
# include <ModbusRTUMaster.h>

const uint8_t dePin = 13;

ModbusRTUMaster modbus(Serial, dePin);
```

---


### setTimeout()

#### Description
Sets the maximum timeout in milliseconds to wait for a response after sending a request. The default is 100 milliseconds.

#### Syntax
``` C++
modbus.setTimeout(time)
```

#### Parameters
- `time`: the timeout duration in milliseconds. Allowed data types: `unsigned long`.

---


### begin()

#### Description
Sets the data rate in bits per second (baud) for serial transmission.
Optionally it also sets the data configuration. Note, there must be 8 data bits for Modbus RTU communication. The default configuration is 8 data bits, no parity, and one stop bit.

#### Syntax
``` C++
modbus.begin(baud)
modbus.begin(baud, config)
modbus.begin(baud, config, rxPin, txPin, invert) // ESP32 only
```

#### Parameters
- `baud`: the baud rate to use for Modbus communication. Common values are: `1200`, `2400`, `4800`, `9600`, `19200`, `38400`, `57600`, and `115200`. Allowed data types: `unsigned long`.
- `config`: the serial port configuration to use. Valid values are:  
`SERIAL_8N1`: no parity (default)  
`SERIAL_8N2`  
`SERIAL_8E1`: even parity  
`SERIAL_8E2`  
`SERIAL_8O1`: odd parity  
`SERIAL_8O2`
- `rxPin`: the pin on which to receive serial data. Allowed data types: `int8_t`, `char`.
- `txPin`: the pin on which to transmit serial data. Allowed data types: `int8_t`, `char`.
- `invert`: will invert RX/TX polarity. Allowed data types: `bool`.

_If using a SoftwareSerial port a configuration of `SERIAL_8N1` will be used regardless of what is entered._  
_The `rxPin`, `txPin`, and `invert` parameters are only available on ESP32 boards._

---


### readCoils()

#### Description
reads coil values from a slave/server device.

#### Syntax
``` C++
modbus.readCoils(slaveId, startAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first coil to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read coil values. Allowed data types: array of `bool`.
- `quantity`: the number of coil values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
bool coils[8];
modbus.readCoils(1, 0, coils, 8);
```

---


### readDiscreteInputs()

#### Description
reads discrete input values from a slave/server device.

#### Syntax
``` C++
modbus.readDiscreteInputs(slaveId, startAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first discrete input to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read discrete input values. Allowed data types: array of `bool`.
- `quantity`: the number of discrete input values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
bool discreteInputs[8];
modbus.readDiscreteInputs(1, 0, discreteInputs, 8);
```

---


### readHoldingRegisters()

#### Description
reads holding register values from a slave/server device.

#### Syntax
``` C++
modbus.readHoldingRegisters(slaveId, startAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first holding register to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read holding register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of holding register values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
uint16_t holdingRegisters[8];
modbus.readHoldingRegisters(1, 0, holdingRegisters, 8);
```

---


### readInputRegisters()

#### Description
reads input register values from a slave/server device.

#### Syntax
``` C++
modbus.readInputRegisters(slaveId, startAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first input register to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read input register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of input register values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
uint16_t inputRegisters[8];
modbus.readInputRegisters(1, 0, inputRegisters, 8);
```

---


### writeSingleCoil()

#### Description
writes a single coil value to a slave/server device.

#### Syntax
``` C++
modbus.writeSingleCoil(slaveId, address, value)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `address`: the address of the coil to write to. Allowed data types: `uint16_t`.
- `value`: the value to write to the coil. Allowed data types: `bool`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

---


### writeSingleHoldingRegister()

#### Description
writes a single holding register value to a slave/server device.

#### Syntax
``` C++
modbus.writeSingleHoldingRegister(slaveId, address, value)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `address`: the address of the holding register to write to. Allowed data types: `uint16_t`.
- `value`: the value to write to the holding register. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

---


### writeMultipleCoils()

#### Description
writes multiple coil values to a slave/server device.

#### Syntax
``` C++
modbus.writeMultipleCoils(slaveId, startingAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first coil to write to. Allowed data types: `uint16_t`.
- `buffer`: an array of coil values. Allowed data types: array of `bool`.
- `quantity`: the number of coil values to write. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
bool coils[2] = {true, false};
modbus.writeMultipleCoils(1, 0, coils, 2);
```

---


### writeMultipleHoldingRegisters()

#### Description
writes multiple holding register values to a slave/server device.

#### Syntax
``` C++
modbus.writeMultipleHoldingRegisters(slaveId, startingAddress, buffer, quantity)
```

#### Parameters
- `slaveId`: the id number of the device to send this request to. Allowed data types: `uint8_t` or `byte`.
- `startAddress`: the address of the first holding register to write to. Allowed data types: `uint16_t`.
- `buffer`: an array of holding register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of holding register values to write. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
`true` if the request was sent and a valid response was received. `false` otherwise. Data type: `bool`.

#### Example
``` C++
uint16_t holdingRegisters[2] = {42, 328};
modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2);
```

---


### getTimeoutFlag()

#### Description
Checks whether a timeout has occured since the last time the flag was cleared.
This flag is set whenever a timeout occurs and cleared when the `clearTimeoutFlag()` function is called.

#### Syntax
``` C++
modbus.getTimeoutFlag()
```

#### Parameters
None

#### Returns
The current value of the flag. Data type: `bool`.

---


### clearTimeoutFlag()

#### Description
Clears the timeout flag.

#### Syntax
``` C++
modbus.clearTimeoutFlag()
```

#### Parameters
None

---


### getExceptionResponse()

#### Description
Checks the last exception response that has occured since the `clearExceptionResponse()` function was called.

When a valid Modbus request is sent, and the recipient slave/server device cannot process it, it responds with an exception code.
This response is called an exception resonse, and it can be helpful in diagnosing issues.

#### Syntax
``` C++
modbus.getExceptionResponse()
```

#### Parameters
None

#### Returns
Exception code. Data type: `uint8_t`.

- 0: none
- 1: illegal function
- 2: illegal data address
- 3: illegal data value
- 4: server device failure

_Details on exeption responses can be found in the [MODBUS Application Protocol Specification](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf). More exeption responses exist than are listed here, but these are the most common._

---


### clearExceptionResponse()

#### Description
clears the exeption response variable.

#### Syntax
``` C++
modbus.clearExceptionResponse()
```

#### Parameters
None
