int pin_GAS = A3;

uint16_t ADC_Data, PPM;

void setup() {
  Serial.begin(115200);

  pinMode(pin_GAS, INPUT);
}

void loop() {
  ADC_Data = analogRead(pin_GAS)/0.6; // ADC & 3V -> 5V level
  PPM = GAS_tr(ADC_Data);  // ADC_data -> ppm

  Serial.print("\n\rADC_Data : ");
  Serial.print(ADC_Data);
  Serial.print(", ");
  Serial.print(PPM);
  Serial.print("[ppm]");
  delay(500);
}

uint16_t GAS_tr(uint16_t adc_data)
{
  float ratio; 
  uint16_t ppm;
  ratio = adc_data;
  ratio = ratio/(1023-adc_data);  // Ro/Rs = Vo/Vs = adc_data/(1023-adc_data)
  if(ratio > 6.7)
  {
    ratio = 6.7;
  }
  ppm = pow(2.71828182,0.7201*ratio)*119.37; // e=2.71828182
  if(ppm > 10000)
  {
    ppm = 10000;
  }
  return ppm;
}

