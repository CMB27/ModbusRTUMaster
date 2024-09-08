# ModbusRTUMasterProbe

This sketch turns the Arduino into a Modbus master/client device that can be used to probe a modbus network.

## RS-485 Circuit:
```
                            VCC                                                                 0.1µF
                             ^                            Arduino Board                        /
                             |                           /                                  | |
 +---------------------------o--------------------------/-------------------------------o---| |---+
 |                                                     /                                |   | |   |
 |         +------+                   +------+        /                                 |         |
 |      +--|      |-------------------|      |-------+                                  |         |
 |      |  |      |                   +------+       |                                  |         |
 |      |  |      |                                  |                                  |         |
 |      |  +------+                          SCL [ ] |                                  |         |               120 Ω
 |      |                                    SDA [ ] |              +-------v-------+   |         |        _____ /
 |      |                                   AREF [ ] |      +-------| RO        VCC |---+         |   +---|_____|---+
 |      |                                    GND [ ] |      |       |               |             |   |             |
 |      | [ ] BOOT                            13 [ ] |------|---o---| RE          B |-------------|---o-------------|---<> RS485_D-
 +------| [ ] IOREF                           12 [ ] |      |   |   |               |             |                 |
        | [ ] RESET                          ~11 [ ] |      |   +---| DE          A |-------------|-----------------o---<> RS485_D+
        | [ ] 3.3V                           ~10 [ ] |      |       |               |             |
        | [ ] 5V                              ~9 [ ] |      |   +---| DI        GND |-------------o
        | [ ] GND                              8 [ ] |      |   |   +---------------+             |
 +------| [ ] GND                                    |      |   |             \                   |
 |      | [ ] VIN                              7 [ ] |      |   |              RS-485             |
 |      |                                     ~6 [ ] |      |   |              Transceiver        |
 |      | [ ] A0                              ~5 [ ] |      |   |                                 |
 |      | [ ] A1                               4 [ ] |      |   |                                 |
 |      | [ ] A2                              ~3 [ ] |      |   |                                 |
 |      | [ ] A3                               2 [ ] |      |   |                                 |
 |      | [ ] A4                            TX→1 [ ] |------|---+                                 |
 |      | [ ] A5                            RX←0 [ ] |------+                                     |
 |      |                                 __________/                                             |
 |       \_______________________________/                                                        |
 |                                                                                                |
 |                                                                                                |
 +------------------------------------------------------------------------------------------------o---------------------<> GND
```

**Components:**  
- Arduino Board
- Half-Duplex RS-485 Transceiver (Must be able to operate at your arduino board's logic level)
- 120 Ω Resistor [^1] (At least 1/4W recommended)

[^1]: The 120 Ω Resistor is only needed at the ends of the RS-485 bus.


**If using an ATmega328P or ATmega168 based board (Arduino UNO R3 and earlier, Arduino Nano, Arduino Pro Mini, etc.):**
- Connect RO to pin 10 instead of pin 0
- Connect DI to pin 11 instead of pin 1

**If using an Arduino Mega (either variant) or Arduino Due:**
- Connect RO to pin 19 instead of pin 0
- Connect DI to pin 18 instead of pin 1

**Connect the following points to the Modbus RS-485 network you want to probe:**
- RS485_D-
- RS485_D+
- GND