/*
  ModbusRTUMasterExample
  
  This example demonstrates how to setup and use the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).
  It is intended to be used with a second board running ModbusRTUSlaveExample from the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave).  
  
  Circuit:
  - The center pin of a potentiometer to pin A0, and the outside pins of the potentiometer to your board's logic level voltage (5V or 3.3V) and GND
  - The center pin of a potentiometer to pin A1, and the outside pins of the potentiometer to your board's logic level voltage (5V or 3.3V) and GND
  - A pushbutton switch from pin 2 to GND
  - A pushbutton switch from pin 3 to GND
  - A LED from pin 5 to GND with a 1K ohm series resistor
  - A LED from pin 6 to GND with a 1K ohm series resistor
  - A LED from pin 7 to GND with a 1K ohm series resistor
  - A LED from pin 8 to GND with a 1K ohm series resistor
  - RX pin (typically pin 0 or pin 10 if using SoftwareSerial) to TX pin of the master/client board
  - TX pin (typically pin 1 or pin 11 if using SoftwareSerial) to RX pin of the master/client board
  - GND to GND of the master/client board
  - Pin 13 is set up as the driver enable pin. This pin will be HIGH whenever the board is transmitting.

  !NOTE
  Both boards will also need power.
  
  Created: 2023-07-22
  By: C. M. Bulliner
  Last Modified: 2024-02-29
  By: C. M. Bulliner
  
*/

#if __AVR__
  // Uncomment the following line if you want to use SoftwareSerial on pins 10 and ll; note this only works on AVR boards.
  //# define USE_SOFTWARE_SERIAL
#endif

// Comment out the following line if you want to disable debugging; note debugging is automatically disabled on the Arduino UNO and Arduino Mega when not using softwareSerial
#define DEBUGGING_ENABLED

#if defined USE_SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
#endif

#include <ModbusRTUMaster.h>

const byte potPins[2] = {A0, A1};
const byte buttonPins[2] = {2, 3};
const byte ledPins[4] = {5, 6, 7, 8};
const byte dePin = 13;

#if defined USE_SOFTWARE_SERIAL
  const byte rxPin = 10;
  const byte txPin = 11;
  SoftwareSerial mySerial(rxPin, txPin);
  ModbusRTUMaster modbus(mySerial, dePin); // serial port, driver enable pin for rs-485
#else
  #if (defined __AVR_ATmega328P__ || defined __AVR_ATmega168__ || defined __AVR_ATmega1280__ || defined __AVR_ATmega2560__)
    ModbusRTUMaster modbus(Serial, dePin); // serial port, driver enable pin for rs-485
    #undef DEBUGGING_ENABLED
  #elif defined ESP32
    ModbusRTUMaster modbus(Serial0, dePin); // serial port, driver enable pin for rs-485
  #else
    ModbusRTUMaster modbus(Serial1, dePin); // serial port, driver enable pin for rs-485
  #endif
#endif

bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

void setup() {
  #if defined ESP32
    analogReadResolution(10);
  #endif

  pinMode(potPins[0], INPUT);
  pinMode(potPins[1], INPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);
  
  modbus.begin(38400);

  #if defined DEBUGGING_ENABLED
    Serial.begin(115200);
    while (!Serial);
  #endif
}

void loop() {
  holdingRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);

  #if defined DEBUGGING_ENABLED
    debug(modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2)); // slave id, starting data address, unsigned 16 bit integer array of holding register values, number of holding registers to write
    debug(modbus.writeMultipleCoils(1, 0, coils, 2));                       // slave id, starting data address, bool array of coil values, number of coils to write
    debug(modbus.readInputRegisters(1, 0, inputRegisters, 2));              // slave id, starting data address, unsigned 16 bit integer array to place input register values, number of input registers to read
    debug(modbus.readDiscreteInputs(1, 0, discreteInputs, 2));              // slave id, starting data address, bool array to place discrete input values, number of discrete inputs to read
  #else
    modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2); // slave id, starting data address, unsigned 16 bit integer array of holding register values, number of holding registers to write
    modbus.writeMultipleCoils(1, 0, coils, 2);                       // slave id, starting data address, bool array of coil values, number of coils to write
    modbus.readInputRegisters(1, 0, inputRegisters, 2);              // slave id, starting data address, unsigned 16 bit integer array to place input register values, number of input registers to read
    modbus.readDiscreteInputs(1, 0, discreteInputs, 2);              // slave id, starting data address, bool array to place discrete input values, number of discrete inputs to read
  #endif
  
  analogWrite(ledPins[0], inputRegisters[0]);
  analogWrite(ledPins[1], inputRegisters[1]);
  digitalWrite(ledPins[2], discreteInputs[0]);
  digitalWrite(ledPins[3], discreteInputs[1]);
}

# if defined DEBUGGING_ENABLED
  bool debug(bool modbusRequest) {
    if (modbusRequest == true) {
      Serial.println("Success");
    }
    else {
      Serial.print("Failure");
      if (modbus.getTimeoutFlag() == true) {
        Serial.print(": Timeout");
        modbus.clearTimeoutFlag();
      }
      else if (modbus.getExceptionResponse() != 0) {
        Serial.print(": Exception Response ");
        Serial.print(modbus.getExceptionResponse());
        switch (modbus.getExceptionResponse()) {
          case 1:
            Serial.print(" (Illegal Function)");
            break;
          case 2:
            Serial.print(" (Illegal Data Address)");
            break;
          case 3:
            Serial.print(" (Illegal Data Value)");
            break;
          case 4:
            Serial.print(" (Server Device Failure)");
            break;
          default:
            Serial.print(" (Uncommon Exception Response)");
            break;
        }
        modbus.clearExceptionResponse();
      }
      Serial.println();
    }
    Serial.flush();
    return modbusRequest;
  }
#endif
