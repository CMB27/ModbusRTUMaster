# ModbusRTUMaster

Modbus is an industrial communication protocol. The RTU variant communicates over serial lines such as UART, RS-232, or RS-485. The full details of the Modbus protocol can be found at [modbus.org](https://modbus.org). A good summary can also be found on [Wikipedia](https://en.wikipedia.org/wiki/Modbus).  

This is an Arduino library that implements the master/client logic of the Modbus RTU protocol. This library implements function codes [1 (Read Coils)](#readcoils), [2 (Read Discrete Inputs)](#readdiscreteinputs), [3 (Read Holding Registers)](#readholdingregisters), [4 (Read Input Registers)](#readinputregisters), [5 (Write Single Coil)](#writesinglecoil), [6 (Write Single Holding Register)](#writesingleholdingregister), [15 (Write Multiple Coils)](#writemultipleholdingregisters), and [16 (Write Multiple Holding Registers)](#writemultipleholdingregisters).  

This library will work with any `Stream` object, like `Serial`. A driver enable pin can be set up, enabling a half-duplex RS-485 transceiver to be used. Only `SERIAL_8N1`, `SERIAL_8E1`, `SERIAL_8O1`, `SERIAL_8N2`, `SERIAL_8E2`, and `SERIAL_8O2` configurations are supported; attempting to use any other configuration will cause the library to default to timings for `SERIAL_8N1`.  

## Version Note
There are some significant changes going from version 1.x.x to version 2.x.x of this library.  
- `begin()` for the Serial object used needs to be run before running `begin()` for the library itself, e.g.:
  ```C++
  Serial1.begin(38400);
  modbus.begin(38400);
  ```
- The read and write functions return an error value instead of a pass/fail `bool`.
- The `getTimeoutFlag()`, `clearTimeoutFlag()`, and `clearExceptionResponse()` functions have been removed.
- This library is also now dependent on [ModbusADU](https://github.com/CMB27/ModbusADU) and [ModbusRTUComm](https://github.com/CMB27/ModbusRTUComm).


## Compatibility
This library has been tested with the following boards and cores:

| Board Name                  | Core                                                                 | Works   |
| :-------------------------- | :------------------------------------------------------------------- | :-----: |
| Arduino Due                 | **Arduino SAM Boards (32-bits ARM Cortex-M3)** by Arduino `1.6.12`   | Yes     |
| Arduino Giga                | **Arduino Mbed OS GIGA Boards** by Arduino `4.1.5`                   | Yes     |
| Arduino Leonardo            | **Arduino AVR Boards** by Arduino `1.8.6`                            | Yes     |
| Arduino Make Your UNO       | **Arduino AVR Boards** by Arduino `1.8.6`                            | Yes     |
| Arduino Mega 2560           | **Arduino AVR Boards** by Arduino `1.8.6`                            | Yes     |
| Arduino Nano                | **Arduino AVR Boards** by Arduino `1.8.6`                            | Yes     |
| Arduino Nano 33 BLE         | **Arduino Mbed OS Nano Boards** by Arduino `4.1.5`                   | Yes     |
| Arduino Nano 33 IoT         | **Arduino SAMD Boards (32-bits ARM Cortex-M0+)** by Arduino `1.8.14` | Yes     |
| Arduino Nano ESP32          | **Arduino ESP32 Boards** by Arduino `2.0.13`                         | Yes     |
| Arduino Nano ESP32          | **esp32** by Espressif Systems `3.0.4`                               | Yes     |
| Arduino Nano Every          | **Arduino megaAVR Boards** by Arduino `1.8.8`                        | Yes     |
| Arduino Nano Matter         | **Silicon Labs** by Silicon Labs `2.1.0`                             | No [^1] |
| Arduino Nano RP2040 Connect | **Arduino Mbed OS Nano Boards** by Arduino `4.1.5`                   | No [^2] |
| Arduino Nano RP2040 Connect | **Raspberry Pi Pico/RP2040** by Earle F. Philhower, III `4.0.1`      | Yes     |
| Arduino UNO R3 SMD          | **Arduino AVR Boards** by Arduino `1.8.6`                            | Yes     |
| Arduino UNO R4 Minima       | **Arduino UNO R4 Boards** by Arduino `1.2.0`                         | Yes     |

[^1]: **Arduino Nano RP2040 Connect**  
This board has trouble receiving Modbus messages when using the `Arduino Mbed OS Nano Boards` core by Arduino.  
It seems that there is some sort of timing issue.  
It can technically be made to work if you tell the library that it is operating at a lower baud rate than the serial port assigned to the library is actually operating at.
However, this would cause the library to operate with unknown timing tolerances, possibly well outside the Modbus specification.

[^2]: **Arduino Nano Matter**  
As of this writing (2024-09-07), `flush()` is not properly implemented with Serial on this board.  
ModbusRTUMaster depends on `flush()` to know when to set the DE and RE pins LOW after a message is sent.


## Examples
- [ModbusRTUMasterExample](https://github.com/CMB27/ModbusRTUMaster/blob/main/examples/ModbusRTUMasterExample/ModbusRTUMasterExample.ino)
- [ModbusRTUMasterProbe](https://github.com/CMB27/ModbusRTUMaster/blob/main/examples/ModbusRTUMasterProbe/ModbusRTUMasterProbe.ino)

## Methods

### ModbusRTUMaster()

#### Description
Creates a ModbusRTUMaster object and sets the serial port to use for data transmission.
Optionally sets a driver enable pin. This pin will go `HIGH` when the library is transmitting. This is primarily intended for use with an RS-485 transceiver, but it can also be a handy diagnostic when connected to an LED.

#### Syntax
``` C++
ModbusRTUMaster(serial)
ModbusRTUMaster(serial, dePin)
ModbusRTUMaster(serial, dePin, rePin)
```

#### Parameters
- `serial`: the `Stream` object to use for Modbus communication. Usually something like `Serial1`.
- `dePin`: the driver enable pin. This pin is set HIGH when transmitting. If this parameter is set to `-1`, this feature will be disabled. The default value is `-1`. Allowed data types are `int8_t` or `char`.
- `rePin`: works exacly the same way as `dePin`. This option is included for compatibility with RS-485 shields like the [Arduino MKR 485 Shield](https://store.arduino.cc/products/arduino-mkr-485-shield).

#### Example
``` C++
# include <ModbusRTUMaster.h>

const int8_t dePin = A6;
const int8_t rePin = A5;

ModbusRTUMaster modbus(Serial1, dePin, rePin);
```

---


### setTimeout()

#### Description
Sets the maximum timeout in milliseconds to wait for a response after sending a request. The default is 500 milliseconds.

#### Syntax
``` C++
modbus.setTimeout(timeout)
```

#### Parameters
- `timeout`: the timeout duration in milliseconds. Allowed data types: `unsigned long`.

---


### begin()

#### Description
Sets the data rate in bits per second (baud) for serial transmission.
Optionally it also sets the data configuration. Note, there must be 8 data bits for Modbus RTU communication. The default configuration is 8 data bits, no parity, and one stop bit.

#### Syntax
``` C++
modbus.begin(baud)
modbus.begin(baud, config)
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
 
*In the 2.0.0+ version of this library, `begin()` for the serial port used with the modbus object must be run seperately.*

#### Example
``` C++
void setup() {
  Serial1.begin(38400, SERIAL_8E1);
  modbus.begin(38400, SERIAL_8E1)
}
```

---


### readCoils()

#### Description
reads coil values from a slave/server device.

#### Syntax
``` C++
modbus.readCoils(unitId, startAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `1` - `246`.
- `startAddress`: the address of the first coil to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read coil values. Allowed data types: array of `bool`.
- `quantity`: the number of coil values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `12`: Unexpected byte count in response

#### Example
``` C++
bool coils[8];
uint8_t error = modbus.readCoils(1, 0, coils, 8);
```

---


### readDiscreteInputs()

#### Description
reads discrete input values from a slave/server device.

#### Syntax
``` C++
modbus.readDiscreteInputs(unitId, startAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `1` - `246`.
- `startAddress`: the address of the first discrete input to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read discrete input values. Allowed data types: array of `bool`.
- `quantity`: the number of discrete input values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `12`: Unexpected byte count in response

#### Example
``` C++
bool discreteInputs[8];
uint8_t error = modbus.readDiscreteInputs(1, 0, discreteInputs, 8);
```

---


### readHoldingRegisters()

#### Description
reads holding register values from a slave/server device.

#### Syntax
``` C++
modbus.readHoldingRegisters(unitId, startAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `1` - `246`.
- `startAddress`: the address of the first holding register to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read holding register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of holding register values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `12`: Unexpected byte count in response

#### Example
``` C++
uint16_t holdingRegisters[8];
uint8_t error = modbus.readHoldingRegisters(1, 0, holdingRegisters, 8);
```

---


### readInputRegisters()

#### Description
reads input register values from a slave/server device.

#### Syntax
``` C++
modbus.readInputRegisters(unitId, startAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `1` - `246`.
- `startAddress`: the address of the first input register to read. Allowed data types: `uint16_t`.
- `buffer`: an array in which to place the read input register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of input register values to read. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `12`: Unexpected byte count in response

#### Example
``` C++
uint16_t inputRegisters[8];
uint8_t error = modbus.readInputRegisters(1, 0, inputRegisters, 8);
```

---


### writeSingleCoil()

#### Description
writes a single coil value to a slave/server device.

#### Syntax
``` C++
modbus.writeSingleCoil(unitId, address, value)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `0` - `246`. `0` indicates a broadcast message.
- `address`: the address of the coil to write to. Allowed data types: `uint16_t`.
- `value`: the value to write to the coil. Allowed data types: `bool`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `13`: Unexpected data address in response
- `14`: Unexpected data value in response

---


### writeSingleHoldingRegister()

#### Description
writes a single holding register value to a slave/server device.

#### Syntax
``` C++
modbus.writeSingleHoldingRegister(unitId, address, value)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `0` - `246`. `0` indicates a broadcast message.
- `address`: the address of the holding register to write to. Allowed data types: `uint16_t`.
- `value`: the value to write to the holding register. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `13`: Unexpected data address in response
- `14`: Unexpected data value in response

---


### writeMultipleCoils()

#### Description
writes multiple coil values to a slave/server device.

#### Syntax
``` C++
uint8_t error = modbus.writeMultipleCoils(unitId, startingAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `0` - `246`. `0` indicates a broadcast message.
- `startAddress`: the address of the first coil to write to. Allowed data types: `uint16_t`.
- `buffer`: an array of coil values. Allowed data types: array of `bool`.
- `quantity`: the number of coil values to write. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `13`: Unexpected data address in response
- `15`: Unexpected quantity in response

#### Example
``` C++
bool coils[2] = {true, false};
uint8_t error = modbus.writeMultipleCoils(1, 0, coils, 2);
```

---


### writeMultipleHoldingRegisters()

#### Description
writes multiple holding register values to a slave/server device.

#### Syntax
``` C++
modbus.writeMultipleHoldingRegisters(unitId, startingAddress, buffer, quantity)
```

#### Parameters
- `unitId`: the id number of the device to send this request to. Valid values are `0` - `246`. `0` indicates a broadcast message.
- `startAddress`: the address of the first holding register to write to. Allowed data types: `uint16_t`.
- `buffer`: an array of holding register values. Allowed data types: array of `uint16_t`.
- `quantity`: the number of holding register values to write. This value must not be larger than the size of the array. Allowed data types: `uint16_t`.

#### Returns
Error code. Data type: `ModbusRTUMasterError` or `uint8_t`.

- `0`: Success
- `1`: Invalid unit ID
- `2`: Invalid buffer
- `3`: Invalid quntity
- `4`: Response timeout
- `5`: Frame error
- `6`: CRC error
- `7`: Unknown communication error
- `8`: Unexpected unit ID in response
- `9`: Exception response
- `10`: Unexpected function code in response
- `11`: Unexpected response length
- `13`: Unexpected data address in response
- `15`: Unexpected quantity in response

#### Example
``` C++
uint16_t holdingRegisters[2] = {42, 328};
uint8_t error = modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2);
```

---


### getExceptionResponse()

#### Description
Gets the last exception response that has occured.

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

- `0`: None
- `1`: Illegal function
- `2`: Illegal data address
- `3`: Illegal data value
- `4`: Server device failure

_Details on exeption responses can be found in the [Modbus Application Protocol Specification](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf). More exeption responses exist than are listed here, but these are the most common._

---

*Soli Deo Gloria*
