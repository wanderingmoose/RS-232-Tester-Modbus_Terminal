// This one works great system for testing modems at 9600 baud rate
//Feb 6 2026
//Made this for work.
//Simple using hyper terminal press a 'a' and wait for a response.
//Also can be poll with a modbus RTU master at address 100 for
//7 registers. (32Bit floats)
//

#include <ModbusRTUSlave.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <SoftwareSerial.h> // Include the SoftwareSerial library

//SoftwareSerial mySerial(2, 3);
#define MODBUS_SERIAL Serial
#define MODBUS_BAUD 9600
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_UNIT_ID 100
#define DS18B20_PIN 4
#define POT_PIN A0
#define DE_PIN -1  // No DE pin for RS232 (or use a real pin if needed later)

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

ModbusRTUSlave modbus(MODBUS_SERIAL, DE_PIN);

const uint8_t numHoldingRegisters = 14;
uint16_t holdingRegisters[numHoldingRegisters];

float tempC;
float tempF;
float volt;
const float static1 = 1.2345;
const float static2 = -2.3456;
const float static3 = 123.456;
const float static4 = -2345.60;

void setFloat(uint16_t* regs, uint16_t index, float value) {
  uint32_t u32 = *((uint32_t*)&value);
  regs[index] = (u32 >> 16) & 0xFFFF;  // High word (big-endian word order)
  regs[index + 1] = u32 & 0xFFFF;      // Low word
}

void printValues() {
  Serial.print("Deg C:   ");
  Serial.println(tempC, 4);
  Serial.print("Deg F:   ");
  Serial.println(tempF, 4);
  Serial.println();
  Serial.print("Volts:   ");
  Serial.println(volt, 4);
   Serial.println();
  Serial.println("Test Values:");
  Serial.println(static1, 4);
  Serial.println(static2, 4);
  Serial.println(static3, 4);
  Serial.println(static4, 4);
}

void setup() {
  pinMode(POT_PIN, INPUT);

  sensors.begin();

  // MUST start serial yourself in v3.x+
  MODBUS_SERIAL.begin(MODBUS_BAUD, MODBUS_CONFIG);

  // Now start Modbus slave
  modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD);  // or modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD, MODBUS_CONFIG);

  modbus.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);
}

void loop() {
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  tempF = tempC * 1.8 + 32.0;
  volt = analogRead(POT_PIN) * (5.0 / 1023.0);
  setFloat(holdingRegisters, 0, tempC);
  setFloat(holdingRegisters, 2, tempF);
  setFloat(holdingRegisters, 4, volt);
  setFloat(holdingRegisters, 6, static1);
  setFloat(holdingRegisters, 8, static2);
  setFloat(holdingRegisters, 10, static3);
  setFloat(holdingRegisters, 12, static4);

  // Simple way to detect 'a' vs Modbus traffic on same port
  if (MODBUS_SERIAL.available()) {
    unsigned long startTime = millis();
    while (millis() - startTime < 5) {  // short timeout to allow multi-byte Modbus frames
      if (MODBUS_SERIAL.available() > 1) break;
    }
    if (MODBUS_SERIAL.available() == 1) {
      if (MODBUS_SERIAL.peek() == 'a') {
        MODBUS_SERIAL.read();  // consume the 'a'
        printValues();
      } else {
        modbus.poll();
      }
    } else {
      modbus.poll();
    }
  } else {
    modbus.poll();  // keep polling even when idle
  }
}