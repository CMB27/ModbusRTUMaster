# ModbusRTUMaster
## Description  
This is an Arduino library that implements the master/client logic of the Modbus RTU protocol. This library implements function codes 1 (Read Coils), 2 (Read Discrete Inputs), 3 (Read Holding Registers), 4 (Read Input Registers), 5 (Write Single Coil), 6 (Write Single Holding Register), 15 (Write Multiple Coils), and 16 (Write Multiple Holding Registers).  
## Modbus  
Modbus is an industrial communication protocol. The RTU variant communicates over serial lines such as UART, RS-232, or RS-485. The full details of the Modbus protocol can be found at [modbus.org](https://modbus.org). A good summary can also be found on [Wikipedia](https://en.wikipedia.org/wiki/Modbus).  
## Library Details  
This library will work with HardwareSerial, SoftwareSerial, or Serial_ (USB Serial on ATmega32u4 based boards). A driver enable pin can be set up, enabling an RS-485 transceiver to be used.
