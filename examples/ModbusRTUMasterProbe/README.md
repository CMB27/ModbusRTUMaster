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



## RS-232 Circuit:
```
                            VCC
                             ^                            Arduino Board                   RS-232 Transceiver
                             |                           /                               /
 +---------------------------o--------------------------/-------------------------------/------+
 |                                                     /                               /       |
 |         +------+                   +------+        /                               /        |       5x 1µF Capacitors
 |      +--|      |-------------------|      |-------+                               /         |      /
 |      |  |      |                   +------+       |                   +-------v-------+     |     /
 |      |  |      |                                  |          +---)|---| C1+       VCC |-----o---|(---+
 |      |  +------+                          SCL [ ] |          |        |               |              |  
 |      |                                    SDA [ ] |      +---|---)|---| VS+       GND |--------------o
 |      |                                   AREF [ ] |      |   |        |               |              |
 |      |                                    GND [ ] |      |   +--------| C1-     T1OUT |--------------|-------> RS232_TX
 |      | [ ] BOOT                            13 [ ] |      |            |               |              |
 +------| [ ] IOREF                           12 [ ] |      |   +---)|---| C2+      R1IN |--------------|-------< RS232_RX
        | [ ] RESET                          ~11 [ ] |      |   |        |               |              |
        | [ ] 3.3V                           ~10 [ ] |      |   +--------| C2-     R1OUT |----------+   |
        | [ ] 5V                              ~9 [ ] |      |            |               |          |   |
        | [ ] GND                              8 [ ] |      o-------|(---| VS-      T1IN |------+   |   |
 +------| [ ] GND                                    |      |            |               |      |   |   |
 |      | [ ] VIN                              7 [ ] |      |         x--| T2OUT    T2IN |--x   |   |   |
 |      |                                     ~6 [ ] |      |            |               |      |   |   |
 |      | [ ] A0                              ~5 [ ] |      |         x--| R2IN    R2OUT |--x   |   |   |
 |      | [ ] A1                               4 [ ] |      |            +---------------+      |   |   |
 |      | [ ] A2                              ~3 [ ] |      |                                   |   |   |
 |      | [ ] A3                               2 [ ] |      |                                   |   |   |
 |      | [ ] A4                            TX→1 [ ] |------|-----------------------------------+   |   |
 |      | [ ] A5                            RX←0 [ ] |------|---------------------------------------+   |
 |      |                                 __________/       |                                           |
 |       \_______________________________/                  |                                           |
 |                                                          |                                           |
 |                                                          |                                           |
 +----------------------------------------------------------o-------------------------------------------o------<> GND
```

**Components:**  
- Arduino Board
- RS-232 Transceiver (Must be able to operate at your arduino board's logic level)
- 5x 1µF Capacitors (At least 20V recommended)

**If using an ATmega328P or ATmega168 based board (Arduino UNO R3 and earlier, Arduino Nano, Arduino Pro Mini, etc.):**
- Connect R1OUT to pin 10 instead of pin 0
- Connect T1IN to pin 11 instead of pin 1

**If using an Arduino Mega (either variant) or Arduino Due:**
- Connect R1OUT to pin 19 instead of pin 0
- Connect T1IN to pin 18 instead of pin 1

**Connect the following points to the Modbus RS-232 device you want to probe:**
- RS232_TX to RS232_RX
- RS232_RX to RS232_TX
- GND to GND



## UART Circuit:
```
                                                Arduino Board
    +------+                   +------+        /
 +--|      |-------------------|      |-------+
 |  |      |                   +------+       |
 |  |      |                                  |
 |  +------+                          SCL [ ] |
 |                                    SDA [ ] |
 |                                   AREF [ ] |
 |                                    GND [ ] |------<> GND
 | [ ] BOOT                            13 [ ] |
 | [ ] IOREF                           12 [ ] |
 | [ ] RESET                          ~11 [ ] |
 | [ ] 3.3V                           ~10 [ ] |
 | [ ] 5V                              ~9 [ ] |
 | [ ] GND                              8 [ ] |
 | [ ] GND                                    |
 | [ ] VIN                              7 [ ] |
 |                                     ~6 [ ] |
 | [ ] A0                              ~5 [ ] |
 | [ ] A1                               4 [ ] |
 | [ ] A2                              ~3 [ ] |
 | [ ] A3                               2 [ ] |
 | [ ] A4                            TX→1 [ ] |-------> TX
 | [ ] A5                            RX←0 [ ] |-------< RX
 |                                 __________/
  \_______________________________/

```

**Components:**  
- Arduino Board

**If using an ATmega328P or ATmega168 based board (Arduino UNO R3 and earlier, Arduino Nano, Arduino Pro Mini, etc.):**
- Use pin 10 as RX instead of pin 0
- USE pin 11 as TX instead of pin 1

**If using an Arduino Mega (either variant) or Arduino Due:**
- Use pin 19 as RX instead of pin 0
- Use pin 18 as TX instead of pin 1

**Connect the following points to the device you want to probe:**
- TX to RX
- RX to TX
- GND to GND

> [!WARNING]  
> Make sure the voltage levels are both the same.
> If they are different, you will need a logic level converter.
