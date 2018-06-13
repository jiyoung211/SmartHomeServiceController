int pin_LED1 = 22;
int pin_LED2 = 23;
int pin_SW1 = 24;
int pin_SW2 = 25;

void setup() {
  pinMode(pin_SW1, INPUT);
  pinMode(pin_SW2, INPUT);
  pinMode(pin_LED1, OUTPUT);
  pinMode(pin_LED2, OUTPUT);
}

void loop() {
  digitalWrite(pin_LED1, digitalRead(pin_SW1));
  digitalWrite(pin_LED2, digitalRead(pin_SW2));
  delay(200);
}

