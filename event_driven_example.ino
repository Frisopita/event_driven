//Test de event driven 
//Frida Sofia Andrade Sierra
struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button buttons[] = {
    {15, 0, false},
    {18, 0, false}
};

unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

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

void setup() {
    Serial.begin(115200);

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        pinMode(buttons[i].PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(buttons[i].PIN), isr, FALLING);
    }
}

void loop() {
    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
        if (buttons[i].pressed) {
            Serial.printf("Button %d has been pressed %u times\n", i + 1, buttons[i].numberKeyPresses);
            buttons[i].pressed = false;
        }
    }
}
