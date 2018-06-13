#include "Wire.h"
#define AD7933_adress 0x22

uint32_t time_check = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("\r\nstart");
}

void loop() {
  uint16_t ADC_DATA;
  uint8_t i;
  
  if(time_check < millis())
  {
    time_check = millis() + 1000;
    ADC_DATA = AD_converter(0);
    Serial.print("GAS : ");
    Serial.println(ADC_DATA);
  }
}

uint16_t AD_converter(uint8_t CH)
{
  uint8_t da = 0x00;
  uint16_t ADC_data = 0;
  switch(CH)
  {
    case 0:
      da= 0x10;
      break;
    case 1:
      da = 0x20;
      break;
    case 2:
      da = 0x40;
      break;
    case 3:
      da = 0x80;
      break;
  }
  Wire.beginTransmission(AD7933_adress);
  Wire.write(da);
  Wire.endTransmission();
  Wire.requestFrom(AD7933_adress, 2);
  while(Wire.available())
  {
    ADC_data <<=8;
    ADC_data |= (uint8_t)(Wire.read());
  }
  Wire.endTransmission();
  ADC_data >>=2;
  return ADC_data;
}
