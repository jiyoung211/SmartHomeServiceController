#include "Wire.h"
#define AD7933_adress 0x22
#define index_MAX 20

uint8_t Pulse_Data[index_MAX], Pulse_sum = 0;
uint32_t time_check_20ms = 0, time_check_3s = 0;
uint8_t Pulse_count = 0, Pulse_flag = 0;
uint8_t Index = 0, Index_flag = 0;
uint16_t ADC_DATA_pre[4] = {0,0,0,0};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  time_check_20ms = millis();
  time_check_3s = time_check_20ms;
  
  Serial.println("\r\nstart");
}

void loop() {
  uint16_t ADC_DATA;
  uint8_t i;
  if((time_check_20ms + 20) < millis())
  {
    time_check_20ms += 20;
    ADC_DATA = AD_converter(0);
    if((ADC_DATA < ADC_DATA_pre[0]) && (ADC_DATA_pre[0] < ADC_DATA_pre[1]) && (ADC_DATA_pre[1] < ADC_DATA_pre[2]) && (ADC_DATA_pre[2] < ADC_DATA_pre[3]) && Pulse_flag)
    {
      if((ADC_DATA_pre[3] - ADC_DATA) > 100)
      {
        Pulse_flag = 0;
        Pulse_count++;
      }
    }
    else if((ADC_DATA > ADC_DATA_pre[0]) && (ADC_DATA_pre[0] > ADC_DATA_pre[1]) && (ADC_DATA_pre[1] > ADC_DATA_pre[2]) && (ADC_DATA_pre[2] > ADC_DATA_pre[3]) && (Pulse_flag == 0))
    {
      if((ADC_DATA - ADC_DATA_pre[3]) > 100)
      {
        Pulse_flag =1;
      }
    }
    for(i=3; i>0; i--)
    {
      ADC_DATA_pre[i] = ADC_DATA_pre[i-1];
    }
    ADC_DATA_pre[0] = ADC_DATA;
  }
  if((time_check_3s + 3000) < millis())
  {
    time_check_3s += 3000;
    Pulse_Data[Index] = 0;
    if(Pulse_count <= 8)
    {
      Pulse_Data[Index] = Pulse_count;
    }
    Pulse_count = 0;
    if(Index == (index_MAX -1))
    {
      Index = 0;
    }
    else
    {
      Index++;
    }
    if(Index_flag < index_MAX)
    {
      Index_flag++;
    }
    Pulse_sum = 0;
    for(i=0; i<Index_flag; i++)
    {
      Pulse_sum += Pulse_Data[i];
    }
    if(Index_flag < index_MAX)
    {
      Pulse_sum = Pulse_sum * index_MAX / Index_flag;
    }
    Serial.print("Pulse : ");
    
    if(Pulse_sum < 20)
    {
      Serial.println("Err");
      Index = 0;
      Index_flag = 0;
      time_check_20ms = millis();
      time_check_3s = time_check_20ms;
    }
    else
    {
      Serial.println(Pulse_sum);
    }
  }
}

uint16_t AD_converter(uint8_t CH)
{
  uint8_t da = 0x00;
  uint16_t ADC_data = 0;
  switch(CH)
  {
    case 0:
      da = 0x10;
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
    ADC_data <<= 8;
    ADC_data |= (uint8_t)(Wire.read());
  }
  Wire.endTransmission();
  
  ADC_data >>= 2;
  return ADC_data;
}
