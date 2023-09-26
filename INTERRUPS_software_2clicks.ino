//Test de event driven ESP32 por software
//Frida Sofia Andrade Sierra
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_APDS9960.h>
#include <MCP3X21.h>

#define timeSeconds 10

Adafruit_BME280 bme;
Adafruit_APDS9960 apds;

bool isReadSensorEnabled = true;
int temp = 0;
const int ledPin = 26; 
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int lastGesture = -1;


void IRAM_ATTR detectsChange() {
  digitalWrite(ledPin, HIGH);
  startTimer = true;
  lastTrigger = millis();
}


void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  //wheater
  bool status1 = bme.begin(0x76);
  if (!status1) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1)
      ;
  } else {
    Serial.println("Valid BM280 sensor");
  }

  //Gestos
  bool status2 = apds.begin(0x39);
  if (!status2) {
    Serial.println("Could not find a valid IR Gesture, check wiring!");
    while (1)
      ;
  } else {
    Serial.println("Valid IR Gesture sensor");
  }
  apds.enableProximity(true);
  apds.enableGesture(true);

}

void loop() {
  
  uint8_t gesture = apds.readGesture();

  if (gesture == APDS9960_DOWN) {
    Serial.println("v");
  }
  if (gesture == APDS9960_UP) {
    Serial.println("^");
  }
  if (gesture == APDS9960_LEFT) {
    Serial.println("<");
  }
  if (gesture == APDS9960_RIGHT) {
    Serial.println(">");
  }

  static uint32_t taskTimer1 = 0;
  if (millis() - taskTimer1 > 3000U) {
    temp = bme.readTemperature();
    Serial.print(temp);
    taskTimer1 = millis();
  }

  if (temp > 28 && isReadSensorEnabled == true) {
    isReadSensorEnabled = false;
    Serial.println("Temp mas de 28!!! \n");
    startTimer = true;
    detectsChange();
  }
  now = millis();

  if(startTimer && (now - lastTrigger > (timeSeconds*1000)) && (temp<=28)) {
    Serial.println("Temperatura abajo de 28 \n");
    digitalWrite(ledPin, LOW);
    startTimer = false;
    isReadSensorEnabled = true;
  }
  
}
