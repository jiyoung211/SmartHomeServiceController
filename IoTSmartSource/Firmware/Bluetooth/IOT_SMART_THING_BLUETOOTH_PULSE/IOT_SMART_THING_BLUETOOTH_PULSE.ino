#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23

#define index_MAX 20

int pin_PULSE = A1;

uint8_t TX_Sensor_data[TX_Sensor_Packet_length] = {0x76, 0x30, };
uint8_t TX_Sensor_flag = 1;
uint8_t Pulse_Data[index_MAX], Pulse_sum   = 0;
uint32_t time_check_20ms = 0,   time_check_3s = 0;
uint8_t Pulse_count = 0, Pulse_flag = 0; 
uint8_t Index = 0, Index_flag = 0;
uint16_t ADC_DATA_pre[4] = {0, 0, 0, 0};
uint8_t Pulse = 0;
char Bluetooth_Name[] = "Arduino--";

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
//SPP SerialBT(&Btd); // This will set the name to the defaults: "Arduino" and the pin to "0000"
SPP SerialBT(&Btd, Bluetooth_Name, "1234"); // You can also set the name and pin like so

void setup() {
  // put your setup code here, to run once:
  uint8_t i;
  
  Serial.begin(115200);

  for(i=2; i<(TX_Sensor_Packet_length-1); i++)
  {
    TX_Sensor_data[i] = 0xFF;
  }
  TX_Sensor_data[TX_Sensor_Packet_length-1] = 0x3E;
  
  pinMode(pin_PULSE, INPUT);

  Bluetooth_Name[7] = Group_ID/10 + '0';
  Bluetooth_Name[8] = Group_ID%10 + '0';

  MsTimer2::set(5000, Sensor_Read_ISR);
  MsTimer2::start();

  //USB init
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while(1); //halt
  }
  Serial.print(F("\r\nSPP Bluetooth Library Started"));
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t ADC_DATA;
  uint8_t i;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if((time_check_20ms + 20) < millis())
  {
    time_check_20ms = time_check_20ms + 20;
    ADC_DATA = analogRead(pin_PULSE);
    if((ADC_DATA < ADC_DATA_pre[0]) && ( ADC_DATA_pre[0] <   ADC_DATA_pre[1]) && ( ADC_DATA_pre[1] < ADC_DATA_pre[2])   && ( ADC_DATA_pre[2] < ADC_DATA_pre[3]) && Pulse_flag)
    {
      if((ADC_DATA_pre[3] - ADC_DATA) > 100)
      {
        Pulse_flag = 0;
        Pulse_count++;
      }
    }
    else if((ADC_DATA > ADC_DATA_pre[0]) && ( ADC_DATA_pre[0]   > ADC_DATA_pre[1]) && ( ADC_DATA_pre[1] > ADC_DATA_pre[2])   && ( ADC_DATA_pre[2] > ADC_DATA_pre[3]) && (Pulse_flag == 0))
    {
      if((ADC_DATA - ADC_DATA_pre[3]) > 100)
      {
        Pulse_flag = 1;
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
    time_check_3s = time_check_3s + 3000;
    Pulse_Data[Index] = 0;
    if(Pulse_count <= 8)
    {
      Pulse_Data[Index] = Pulse_count;
    }
    Pulse_count = 0;
    if(Index == (index_MAX - 1))
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
      Pulse_sum = Pulse_sum + Pulse_Data[i];
    }
    if(Index_flag < index_MAX)
    {
      Pulse_sum = Pulse_sum * index_MAX / Index_flag;
    }
    //Serial.print("\n\rPulse : ");
    if(Pulse_sum < 20)
    {
      //Serial.print("Err");
      Index = 0;
      Index_flag = 0;
      time_check_20ms = millis();
      time_check_3s = time_check_20ms;
      Pulse = 0;
    }
    else
    {
      //Serial.print(Pulse_sum);
      if(Pulse_sum > 254)
      {
        Pulse = 254;
      }
      else
      {
        Pulse = Pulse_sum;
      }
    }
  }

  if(SerialBT.connected != 0)
  {
    if(TX_Sensor_flag == 1)
    {
      TX_Sensor_data[2] = Pulse;

      SerialBT.write(TX_Sensor_data, TX_Sensor_Packet_length);
      
      Serial.println("\n\rTX Packet :");
      for(i=0; i<TX_Sensor_Packet_length; i++)
      {
        Serial.print(TX_Sensor_data[i], HEX);
        Serial.write(' ');
      }
      
      TX_Sensor_flag = 0;
    }
  }
}

void Sensor_Read_ISR(void)
{
  TX_Sensor_flag = 1;
}

