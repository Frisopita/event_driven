//Test de event driven
//Frida Sofia Andrade Sierra
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_APDS9960.h>
#include <MCP3X21.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
Adafruit_APDS9960 apds;
MCP3221 mcp3221(0x4D);

unsigned long delayTime;
unsigned long button_time = 0;
unsigned long last_button_time = 0;
const uint16_t ref_voltage = 3300;
int lastGesture = -1;

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button buttons[] = {
  { 15, 0, false },
  { 18, 0, false }
};

void IRAM_ATTR isr() {
  button_time = millis();

  for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
    if (button_time - last_button_time > 250 && digitalRead(buttons[i].PIN) == LOW) {
      buttons[i].numberKeyPresses++;
      buttons[i].pressed = true;
      last_button_time = button_time;
    }
  }
}

void WeatherClick() {
  Serial.println("Hola");
}

void HumClick() {
  Serial.println("Adios");
}


void setup() {
  Serial.begin(115200);
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
  //Humedad
  mcp3221.init();
  Serial.println("Valid HUM sensor");
  //botones
  for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
    pinMode(buttons[i].PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttons[i].PIN), isr, FALLING);
  }
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
  //botones
  for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
    if (buttons[i].pressed) {
      Serial.printf("Button %d has been pressed %u times\n", i + 1, buttons[i].numberKeyPresses);
      buttons[i].pressed = false;
      switch (i) {
        case 0:
          Serial.println("Sensor TEMP");
          Serial.print(bme.readTemperature());
          break;
        case 1:
          Serial.println("Sensor HUM");
          Serial.print(mcp3221.read());
          break;
        default:
          Serial.println("None");
      }
    }
  }
}