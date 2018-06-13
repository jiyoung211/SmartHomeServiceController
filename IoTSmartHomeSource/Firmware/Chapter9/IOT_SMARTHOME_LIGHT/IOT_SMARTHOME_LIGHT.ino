int pin_RELAY0_DETECT = 5;
int pin_RELAY0_CH = 4;
int Relay_state = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_RELAY0_CH, OUTPUT);
  pinMode(pin_RELAY0_DETECT, INPUT);
  digitalWrite(pin_RELAY0_CH, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int da = digitalRead(pin_RELAY0_DETECT);
  if(Relay_state != da)
  {
    Relay_state = da;
    Serial.print("LIGHT : ");
    if(Relay_state)
    {
      Serial.println("OFF");
    }
    else
    {
      Serial.println("ON");
    }
  }
}

void serialEvent(void)
{
  char da = Serial.read();
  switch(da)
  {
    case '1':
      digitalWrite(pin_RELAY0_CH, HIGH);
      break;
    case '0':
      digitalWrite(pin_RELAY0_CH, LOW);
      break;
  }
}
