# ModbusRTUMasterExample
 
This example demonstrates how to setup and use the [ModbusRTUMaster](https://github.com/CMB27/ModbusRTUMaster) library.
This is a communications library, so in order to test it, you will need something to communicate with.
A second board running ModbusRTUSlaveExample from the [ModbusRTUSlave](https://github.com/CMB27/ModbusRTUSlave) library will be needed.
This second board will also need to be setup with the circuit shown below.

**This program has been tested with the following boards:**  

| Board Name                  | Core                                         | By                      | Ver.   | Works | RX Pin | TX Pin | Modbus Port    |
| :-------------------------- | :------------------------------------------- | :---------------------- | -----: | :---: | -----: | -----: | :------------- |
| Arduino Due                 | Arduino SAM Boards (32-bits ARM Cortex-M3)   | Arduino                 | 1.6.12 | Yes   |     19 |     18 | Serial1        |
| Arduino Giga                | Arduino Mbed OS GIGA Boards                  | Arduino                 |  4.1.5 | Yes   |      0 |      1 | Serial1        |
| Arduino Leonardo            | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |      0 |      1 | Serial1        |
| Arduino Make Your UNO       | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |
| Arduino Mega 2560           | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     19 |     18 | Serial1        |
| Arduino Nano                | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |
| Arduino Nano 33 BLE         | Arduino Mbed OS Nano Boards                  | Arduino                 |  4.1.5 | Yes   |      0 |      1 | Serial1        |
| Arduino Nano 33 IoT         | Arduino SAMD Boards (32-bits ARM Cortex-M0+) | Arduino                 | 1.8.14 | Yes   |      0 |      1 | Serial1        |
| Arduino Nano ESP32          | Arduino ESP32 Boards                         | Arduino                 | 2.0.13 | Yes   |      0 |      1 | Serial0        |
| Arduino Nano ESP32          | esp32                                        | Espressif Systems       |  3.0.4 | Yes   |      0 |      1 | Serial0        |
| Arduino Nano Every          | Arduino megaAVR Boards                       | Arduino                 |  1.8.8 | Yes   |      0 |      1 | Serial1        |
| Arduino Nano Matter         | Silicon Labs                                 | Silicon Labs            |  2.1.0 | No    |      0 |      1 | Serial1        |
| Arduino Nano RP2040 Connect | Arduino Mbed OS Nano Boards                  | Arduino                 |  4.1.5 | No    |      0 |      1 | Serial1        |
| Arduino Nano RP2040 Connect | Raspberry Pi Pico/RP2040                     | Earle F. Philhower, III |  4.0.1 | Yes   |      0 |      1 | Serial1        |
| Arduino UNO R3 SMD          | Arduino AVR Boards                           | Arduino                 |  1.8.6 | Yes   |     10 |     11 | SoftwareSerial |
| Arduino UNO R4 Minima       | Arduino UNO R4 Boards                        | Arduino                 |  1.2.0 | Yes   |      0 |      1 | Serial1        |


> [!NOTE] on Arduino Leonardo:  
> Closing a Serial Monitor connected to the board while running this program will cause Modbus communication errors.
> Resetting the board should cause normal Modbus communications to resume.

> [!NOTE] on Arduino Nano ESP32:  
> When using the "esp32" core by Espressif Systems, please set pin numbering to "By GPIO number (legacy)".
> The pin numbering option, "By Arduino pin (default)" does not appear to work correctly with this core.

> [!NOTE] on Arduino Nano RP2040 Connect:  
> When using the "Arduino Mbed OS Nano Boards" core by Arduino, this board has trouble receiving Modbus messages.
> It seems that it is some sort of timing issue.

> [!NOTE] on Arduino Nano Matter:  
> flush() is not properly implemented with Serial on this board.
> ModbusRTUMaster depends on flush() to know when to set the DE and RE pins LOW after a message is sent.

## Circuit:  
```
                                                 VCC                                                                 0.1µF Capacitor
                                                  ^                            Arduino Board                        /
                                                  |                           /                                  | |
 +--------------------o---------------------------o--------------------------/-------------------------------o---| |---+
 |                    |                                                     /                                |   | |   |
 |                    |         +------+                   +------+        /            RS-485               |         |
 |                    |      +--|      |-------------------|      |-------+        Transceiver               |         |
 |                    |      |  |      |                   +------+       |                    \             |         |               120 Ω Resistor
 |                    |      |  |      |                                  |              +-------v-------+   |         |        _____ /
 |                    |      |  +------+                          SCL [ ] |      +-------| RO        VCC |---+         |   +---|_____|---+
 |                    |      |                                    SDA [ ] |      |       |               |             |   |             |
 |                    |      |                                   AREF [ ] |      |   +---| RE          B |-------------|---o-------------|---<> RS485_D-
 |                    |      |                                    GND [ ] |      |   |   |               |             |                 |
 |                    |      | [ ] BOOT                            13 [ ] |------|---o---| DE          A |-------------|-----------------o---<> RS485_D+
 |                    +------| [ ] IOREF                           12 [ ] |      |       |               |             |
 |                           | [ ] RESET                          ~11 [ ] |      |   +---| DI        GND |-------------o
 |                           | [ ] 3.3V                           ~10 [ ] |      |   |   +---------------+             |
 |                           | [ ] 5V                              ~9 [ ] |      |   |                       _____     |
 |                           | [ ] GND                              8 [ ] |------|---|----------------->|---|_____|----o
 |                    +------| [ ] GND                                    |      |   |                       _____     |
 |                    |      | [ ] VIN                              7 [ ] |------|---|----------------->|---|_____|----o
 |                    |      |                                     ~6 [ ] |------|---|------------+                    |  4x LEDs
 |      +--------------------| [ ] A0                              ~5 [ ] |------|---|---------+  |          _____     |  4x 1K Ω Resistors
 |      |      +-------------| [ ] A1                               4 [ ] |      |   |         |  +---->|---|_____|----o
 |    __v__    |      |      | [ ] A2                              ~3 [ ] |------|---|------+  |             _____     |
 o---|_____|---|------o      | [ ] A3                               2 [ ] |------|---|---+  |  +------->|---|_____|----o
 |             |      |      | [ ] A4                            TX→1 [ ] |------|---+   |  |                          |
 |           __v__    |      | [ ] A5                            RX←0 [ ] |------+       |  |                          |
 +----------|_____|---o      |                                 __________/               |  |             __|__        |
                      |       \_______________________________/                          |  +-------------o   o--------o
       2x 10K Ω       |                                                                  |                             |  2x Pushbutton Switches
    Potentiometers    |                                                                  |                __|__        |
                      |                                                                  +----------------o   o--------o
                      |                                                                                                |
                      +------------------------------------------------------------------------------------------------o---------------------<> GND
```

## Components:  
- Arduino Board
- Half-Duplex RS-485 Transceiver (Must be able to operate at your arduino board's logic level)
- 120 Ω Resistor (At least 1/4W recommended)
- 4x 1K Ω Resistors
- 2x 10K Ω Potentiometers
- 0.1µF Capacitor
- 4x LEDs


> [!NOTE]  
> If using an ATmega328P or ATmega168 based board (Arduino UNO R3 and earlier, Arduino Nano, Arduino Pro Mini, etc.):
> - Connect RO to pin 10 instead of pin 0
> - Connect DI to pin 11 instead of pin 1

> [!NOTE]  
> If using an Arduino Mega (either variant) or Arduino Due:
> - Connect RO to pin 19 instead of pin 0
> - Connect DI to pin 18 instead of pin 1

> [!IMPORTANT]
> Connect the following together from this circuit and the one for ModbusRTUSlaveExample:
> - RS485_D-
> - RS485_D+
> - GND
