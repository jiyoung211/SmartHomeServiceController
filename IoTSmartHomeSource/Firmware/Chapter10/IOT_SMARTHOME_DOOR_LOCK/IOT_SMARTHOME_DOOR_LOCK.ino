int pin_RELAY0_CH = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_RELAY0_CH, OUTPUT);
  digitalWrite(pin_RELAY0_CH, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void serialEvent(void)
{
  char da = Serial.read();
  switch(da)
  {
    case '1':
      digitalWrite(pin_RELAY0_CH, HIGH);
      delay(100);
      digitalWrite(pin_RELAY0_CH, LOW);
      Serial.println("Door Control");
      break;
  }
}
