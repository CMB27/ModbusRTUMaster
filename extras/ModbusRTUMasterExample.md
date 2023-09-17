# ModbusRTUSlave Example Sketch
This example demonstrates how to setup and use the ModbusRTUMaster and the [ModbusRTUSlave](https://github.com/CMB27/ModbusRTUSlave) library.

For this example to work [ModbusRTUMasterExample](https://github.com/CMB27/ModbusRTUMaster/blob/main/examples/ModbusRTUMasterExample/ModbusRTUMasterExample.ino) needs to be uploaded to one of the Arduino boards and [ModbusRTUSlaveExample](https://github.com/CMB27/ModbusRTUSlave/blob/main/examples/ModbusRTUSlaveExample/ModbusRTUSlaveExample.ino) needs to be uploaded to the other.

If everything works properly, you should be able to control the LEDs connected to one of the boards with the buttons and potentiometers connected to the other board.

## Hardware Required
- 2 Arduino Boards
- 4 Pushbutton Switches
- 8 LEDs
- 8 330 Ohm Resistors
- 4 10K Ohm Potentiometers
- Wires

## Circuit
![](./ModbusRTUMasterSlaveExampleBreadBoard.svg)  
_Image deveolped using [Fritzing](https://fritzing.org/) and [Inkscape](https://inkscape.org/)._

## Schematic
![](./ModbusRTUMasterSlaveExampleSchematic.svg)  
_Note, this schematic only shows half the circuit. The other half is identical._  
_Image developed using [KiCad](https://www.kicad.org/) and [Inkscape](https://inkscape.org/)._

## Code
``` C++
/*
  ModbusRTUMasterExample
  
  This example demonstrates how to setup and use the ModbusRTUMaster library.
  It is intended to be used with a second board running ModbusRTUSlaveExample from the ModbusRTUSlave library.  
  
  Circuit:
  - A pushbutton switch from pin 2 to GND
  - A pushbutton switch from pin 3 to GND
  - A LED from pin 5 to GND with a 330 ohm series resistor
  - A LED from pin 6 to GND with a 330 ohm series resistor
  - A LED from pin 7 to GND with a 330 ohm series resistor
  - A LED from pin 8 to GND with a 330 ohm series resistor
  - The center pin of a potentiometer to pin A0, and the outside pins of the potentiometer to 5V and GND
  - The center pin of a potentiometer to pin A0, and the outside pins of the potentiometer to 5V and GND
  
  !!! If your board's logic voltage is 3.3V, use 3.3V instead of 5V; if in doubt use the IOREF pin !!!
  
  - pin 10 to pin 11 of the slave/server board
  - pin 11 to pin 10 of the slave/server board
  - GND to GND of the slave/server board
  
  A schematic and illustration of the circuit is in the extras folder of the ModbusRTUMaster library.

  - Pin 13 is set up as the driver enable pin. This pin will be HIGH whenever the board is transmitting.
  
  Created: 2023-07-22
  By: C. M. Bulliner
  Modified: 2023-07-29
  By: C. M. Bulliner
  
*/

#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>

const byte ledPins[4] = {8, 7, 6, 5};
const byte buttonPins[2] = {3, 2};
const byte potPins[2] = {A0, A1};

const uint8_t rxPin = 10;
const uint8_t txPin = 11;
const uint8_t dePin = 13;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial, dePin); // serial port, driver enable pin for rs-485 (optional)

bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

void setup() {
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(potPins[0], INPUT);
  pinMode(potPins[1], INPUT);
  
  modbus.begin(38400); // baud rate, config (optional)
}

void loop() {
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);
  holdingRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
  
  modbus.writeMultipleCoils(1, 0, coils, 2);                       // slave id, starting data address, bool array of coil values, number of coils to write
  modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2); // slave id, starting data address, unsigned 16 bit integer array of holding register values, number of holding registers to write
  modbus.readDiscreteInputs(1, 0, discreteInputs, 2);              // slave id, starting data address, bool array to place discrete input values, number of discrete inputs to read
  modbus.readInputRegisters(1, 0, inputRegisters, 2);              // slave id, starting data address, unsigned 16 bit integer array to place input register values, number of input registers to read
  
  digitalWrite(ledPins[0], discreteInputs[0]);
  digitalWrite(ledPins[1], discreteInputs[1]);
  analogWrite(ledPins[2], inputRegisters[0]);
  analogWrite(ledPins[3], inputRegisters[1]);
}
```