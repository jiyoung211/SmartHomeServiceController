int pin_DC_P = 9;
int pin_DC_N = 10;
int pin_DC_EN = 11;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(pin_DC_P, OUTPUT); 
  pinMode(pin_DC_N, OUTPUT); 
  pinMode(pin_DC_EN, OUTPUT); 
  digitalWrite(pin_DC_EN, LOW);
  digitalWrite(pin_DC_P, HIGH);
  digitalWrite(pin_DC_N, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void serialEvent(void)
{
  char da = Serial.read();
  if((da >= '0') && (da <= '5'))
  {
    if(da != '0')
    {
      analogWrite(pin_DC_EN, (da-'0') * 25 + 80);
      Serial.print("\n\rSpeed : ");
      Serial.write(da);
    }
    else
    {
      analogWrite(pin_DC_EN, 0);
      Serial.print("\n\rStop");
    }
  }
}

