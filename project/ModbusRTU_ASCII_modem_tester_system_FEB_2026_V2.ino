//FAILED Nothing work on this one Feb 6 2026
#include <ModbusRTUSlave.h>
#include <AltSoftSerial.h>       // << Replace SoftwareSerial with this
#include <OneWire.h>
#include <DallasTemperature.h>

// Pins for AltSoftSerial on Nano/Uno: FIXED to RX=8, TX=9
// (your RS232 module: TX → Nano pin 8, RX → Nano pin 9, GND common)
AltSoftSerial modbusSerial;      // No arguments needed — pins are fixed

#define MODBUS_BAUD 9600
#define MODBUS_UNIT_ID 100

#define DS18B20_PIN 4
#define POT_PIN A0
#define DE_PIN -1  // Still no DE for RS232

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

ModbusRTUSlave modbus(modbusSerial, DE_PIN);

const uint8_t numHoldingRegisters = 14;
uint16_t holdingRegisters[numHoldingRegisters];

float tempC, tempF, volt;
const float static1 = 1.2345;
const float static2 = -2.3456;
const float static3 = 123.456;
const float static4 = 0.0;

void setFloat(uint16_t* regs, uint16_t index, float value) {
  uint32_t u32 = *((uint32_t*)&value);
  regs[index]     = (u32 >> 16) & 0xFFFF;
  regs[index + 1] = u32 & 0xFFFF;
}

void printValues() {
  modbusSerial.println(tempC, 4);
  modbusSerial.println(tempF, 4);
  modbusSerial.println(volt, 4);
  modbusSerial.println(static1, 4);
  modbusSerial.println(static2, 4);
  modbusSerial.println(static3, 4);
  modbusSerial.println(static4, 4);
  modbusSerial.println();  // extra blank line for readability
}

void setup() {
  Serial.begin(115200);  // USB debug — optional but very helpful now
  Serial.println("Starting Modbus tester with AltSoftSerial...");

  pinMode(POT_PIN, INPUT);
  sensors.begin();

  modbusSerial.begin(MODBUS_BAUD);  // Only baud — no config needed

  modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD);

  modbus.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);

  Serial.println("Ready - slave ID 100 @ 9600 8N1 on pins 8/9");
}

void loop() {
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  tempF = tempC * 1.8 + 32.0;
  volt  = analogRead(POT_PIN) * (5.0 / 1023.0);

  setFloat(holdingRegisters, 0, tempC);
  setFloat(holdingRegisters, 2, tempF);
  setFloat(holdingRegisters, 4, volt);
  setFloat(holdingRegisters, 6, static1);
  setFloat(holdingRegisters, 8, static2);
  setFloat(holdingRegisters, 10, static3);
  setFloat(holdingRegisters, 12, static4);

  static String cmd = "";
  static unsigned long lastCharMs = 0;

  if (modbusSerial.available()) {
    char c = modbusSerial.read();
    cmd += c;
    lastCharMs = millis();
  }

  // Process if idle for 40 ms (increased slightly for reliability)
  if (cmd.length() > 0 && (millis() - lastCharMs >= 40)) {
    cmd.trim();
    // Remove trailing CR/LF if present
    while (cmd.endsWith("\r") || cmd.endsWith("\n")) {
      cmd.remove(cmd.length() - 1);
    }
    cmd.trim();

    Serial.print("Received cmd: '"); Serial.print(cmd); Serial.println("'");  // Debug to USB

    if (cmd == "a" || cmd == "test") {
      printValues();
    }
    // else: ignore (Modbus poll will handle binary frames below)

    cmd = "";  // reset
  }

  modbus.poll();  // Always poll — library reads from modbusSerial directly
}