#include <Wire.h>
#include <SHT2x.h>

int pin_CDS = A2;

uint16_t ADC_Data[20] = {3, 45, 75, 90, 109, 123, 135, 144, 157, 169, 178, 241, 281, 322, 346, 365, 393, 402, 409, 422};
uint16_t LUX_Data[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

void setup() {
  Serial.begin(115200);
  
  pinMode(pin_CDS, INPUT);

  Wire.begin();
}

void loop() {
  uint16_t LUX = 0;
  int8_t Temp = 0;
  uint8_t Humi = 0;
  
  Humi = SHT2x.readRH();
  Temp = SHT2x.readT();
  
  LUX = ADC_TO_LUX(analogRead(pin_CDS));

  Serial.print("\n\rHumi : ");
  Serial.print(Humi);
  Serial.print("[%], Temp : ");
  Serial.print(Temp);
  Serial.print("[C], CDS : ");
  Serial.print(LUX);
  Serial.print("[lux]");
  delay(500);
}

uint16_t ADC_TO_LUX(uint16_t ADC_data)
{
  uint16_t Lux = 0, i;
  for(i=0; i<20; i++)
  {
    if(ADC_data < ADC_Data[i])
    {
      break;
    }
  }
  if(i < 2)
  {
    Lux = 0;
  }
  else if(i < 11)
  {
    Lux = LUX_Data[i-1];
  }
  else if(i > 19)
  {
    Lux = 100;
  }
  else
  {
    Lux = LUX_Data[i-1] + (LUX_Data[i]-LUX_Data[i-1]) * (ADC_data - ADC_Data[i-1]) / (ADC_Data[i]-ADC_Data[i-1]);
  }
  
  return Lux;
}
