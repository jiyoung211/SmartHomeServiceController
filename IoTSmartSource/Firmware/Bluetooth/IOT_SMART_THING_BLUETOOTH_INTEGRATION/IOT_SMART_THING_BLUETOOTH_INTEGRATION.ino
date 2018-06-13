#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>
#include <Wire.h>
#include <SHT2x.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23
#define TX_Event_Packet_length 7
#define RX_Packet_length 9

#define index_MAX 20

int pin_PULSE = A1;
int pin_CDS = A2;
int pin_GAS = A3;
int pin_DUST_LED = 8;
int pin_DUST_OUT = A7;
int pin_RAIN_STEAM = A4;
int pin_SOIL_MOISTURE = A5;
int pin_LED[2] = {22, 23};
int pin_SW[2] = {24, 25};
int pin_TILT = 2;
int pin_SOUND = 3;
int pin_PIR = 19;
int pin_DC_P = 9;
int pin_DC_N = 10;
int pin_DC_EN = 11;

uint8_t TX_Sensor_data[TX_Sensor_Packet_length] = {0x76, 0x30, };
uint8_t TX_Event_data[TX_Event_Packet_length] = {0x76, 0x31, };
uint8_t TX_Sensor_flag = 1, TX_Event_flag = 0, Sound_flag = 0, PIR_flag = 0;
uint8_t RX_data[RX_Packet_length];
uint8_t Pulse_Data[index_MAX], Pulse_sum   = 0;
uint32_t time_check_20ms = 0,   time_check_3s = 0;
uint8_t Pulse_count = 0, Pulse_flag = 0; 
uint8_t Index = 0, Index_flag = 0;
uint16_t ADC_DATA_pre[4] = {0, 0, 0, 0};
uint8_t Pulse = 0;
uint8_t SW_pre = 0, Tilt_Value = 0, Tilt_pre = 0, Sound_pre = 0, PIR_pre = 0;
uint32_t Time_check_Sound = 0, Time_check_PIR = 0, Time_check_Tilt = 0;

uint16_t ADC_Data[20] = {3, 45, 75, 90, 109, 123, 135, 144, 157, 169, 178, 241, 281, 322, 346, 365, 393, 402, 409, 422};
uint16_t LUX_Data[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

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
  
  for(i=2; i<(TX_Event_Packet_length-1); i++)
  {
    TX_Event_data[i] = 0xFF;
  }
  TX_Event_data[TX_Event_Packet_length-1] = 0x3E;

  pinMode(pin_PULSE, INPUT);
  pinMode(pin_CDS, INPUT);
  pinMode(pin_GAS, INPUT);
  pinMode(pin_DUST_LED, OUTPUT);
  pinMode(pin_DUST_OUT, INPUT);
  digitalWrite(pin_DUST_LED, HIGH);
  pinMode(pin_RAIN_STEAM, INPUT);
  pinMode(pin_SOIL_MOISTURE, INPUT);
  for(i=0; i<2; i++)
  {
    pinMode(pin_SW[i], INPUT);
    pinMode(pin_LED[i], OUTPUT);
    digitalWrite(pin_LED[i], LOW);
  }
  pinMode(pin_TILT, INPUT_PULLUP);
  pinMode(pin_SOUND, INPUT);
  pinMode(pin_PIR, INPUT);
  pinMode(pin_DC_P, OUTPUT); 
  pinMode(pin_DC_N, OUTPUT); 
  pinMode(pin_DC_EN, OUTPUT); 
  digitalWrite(pin_DC_EN, LOW);
  digitalWrite(pin_DC_P, HIGH);
  digitalWrite(pin_DC_N, LOW);

  Wire.begin();

  attachInterrupt(digitalPinToInterrupt(pin_TILT), TILT_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin_SOUND), SOUND_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_PIR), PIR_ISR, RISING);

  Bluetooth_Name[7] = Group_ID/10 + '0';
  Bluetooth_Name[8] = Group_ID%10 + '0';

  MsTimer2::set(50, Sensor_Read_ISR);
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
  static uint8_t rx_count = 0;
  uint16_t ADC_DATA;
  int8_t Temp = 0;
  uint8_t i, Humi = 0;
  char da = 0;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if((time_check_20ms + 20) < millis())
  {
    time_check_20ms += 20;
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
    time_check_3s += 3000;
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
      Pulse_sum += Pulse_Data[i];
    }
    if(Index_flag < index_MAX)
    {
      Pulse_sum = Pulse_sum * index_MAX / Index_flag;
    }
    Serial.print("\n\rPulse : ");
    if(Pulse_sum < 20)
    {
      Serial.print("Err");
      Index = 0;
      Index_flag = 0;
      time_check_20ms = millis();
      time_check_3s = time_check_20ms;
      Pulse = 0;
    }
    else
    {
      Serial.print(Pulse_sum);
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
  
  if(SerialBT.connected)
  {
    // receive data check
    if(SerialBT.available() > 0)
    {
      da = SerialBT.read();

      // receive data Start check
      if((rx_count == 0) && (da == 0x76))
      {
        RX_data[rx_count] = da;
        rx_count++;
      }
      else if(rx_count == 1)
      {
        // CMD check
        if(da == 0x32)
        {
          RX_data[rx_count] = da;
          rx_count++;
        }
        else
        {
          rx_count = 0;
        }
      }
      else if(rx_count > 1)
      {
        RX_data[rx_count] = da;
        rx_count++;
        // Complete data received
        if(rx_count == RX_Packet_length)
        {
          rx_count = 0;
          // END check
          if(RX_data[RX_Packet_length-1] == 0x3E)
          {
            Serial.println("\n\r RX_Data ");
            for(i=0; i<RX_Packet_length; i++)
            {
              Serial.print(RX_data[i], HEX);
              Serial.write(' ');
            }
            
            digitalWrite(pin_LED[0], RX_data[2] & 0x01);
            digitalWrite(pin_LED[1], (RX_data[2]>>1) & 0x01);

            if(RX_data[3] != 0)
            {
              analogWrite(pin_DC_EN, RX_data[3] * 25 + 80);
            }
            else
            {
              analogWrite(pin_DC_EN, 0);
            }
          }
        }
      }
    }
    if(TX_Sensor_flag)
    {
      TX_Sensor_data[2] = Pulse;

      Humi = SHT2x.readRH();
      Temp = SHT2x.readT();
      
      ADC_DATA = ADC_TO_LUX(analogRead(pin_CDS));

      TX_Sensor_data[3] = Temp;     //[C]
      TX_Sensor_data[4] = Humi;     //[%]
      TX_Sensor_data[7] = ADC_DATA; // CDS[lux]

      ADC_DATA = ADC_TO_PPM(analogRead(pin_GAS));

      TX_Sensor_data[12] = ADC_DATA >> 8;   //Gas[ppm]
      TX_Sensor_data[13] = ADC_DATA & 0xFF;

      ADC_DATA = Dust_tr(Dust_Read());

      TX_Sensor_data[10] = ADC_DATA >> 8;   //Dust [ug/m3]
      TX_Sensor_data[11] = ADC_DATA & 0xFF;

      ADC_DATA = analogRead(pin_RAIN_STEAM) / 9.7;

      TX_Sensor_data[8] = ADC_DATA;   // Rain/Steam [%]

      ADC_DATA = analogRead(pin_SOIL_MOISTURE) / 9.7;

      TX_Sensor_data[9] = ADC_DATA;   // Soil Moisture [%]

      SerialBT.write(TX_Sensor_data, TX_Sensor_Packet_length);
      
      Serial.println("\n\rTX Packet :");
      for(i=0; i<TX_Sensor_Packet_length; i++)
      {
        Serial.print(TX_Sensor_data[i], HEX);
        Serial.write(' ');
      }
      
      TX_Sensor_flag = 0;
    }
    if(TX_Event_flag)
    {
      SerialBT.write(TX_Event_data, TX_Event_Packet_length);
      
      Serial.println("\n\rTX Event Packet :");
      for(i=0; i<TX_Event_Packet_length; i++)
      {
        Serial.print(TX_Event_data[i], HEX);
        Serial.write(' ');
      }
      TX_Event_flag = 0;
    }
    if(Sound_flag && (Time_check_Sound < millis()))
    {
      if(Sound_pre != 0)
      {
        Sound_pre = 0;
        TX_Event_data[4] = 0;
        TX_Event_flag = 1;
        Sound_flag = 0;
      }
    }
    if(PIR_flag && (Time_check_PIR < millis()))
    {
      if(PIR_pre != 0)
      {
        PIR_pre = 0;
        TX_Event_data[5] = 0;
        TX_Event_flag = 1;
        PIR_flag = 0;
      }
    }
    if((Time_check_Tilt + 250) < millis())
    {
      if(Tilt_pre != Tilt_Value)
      {
        Tilt_pre = Tilt_Value;
        TX_Event_data[3] = Tilt_Value;
        TX_Event_flag = 1;
      }
    }
  }
}

void Sensor_Read_ISR(void)
{
  static uint16_t Timer_60s = 0;

  uint8_t da = SW_read();
  if(SW_pre != da)
  {
    SW_pre = da;
    TX_Event_data[2] = da;
    TX_Event_flag = 1;
  }

  Timer_60s++;
  if(Timer_60s == 1200)
  {
    Timer_60s = 0;
    TX_Sensor_flag = 1;
  }
}

//Read Dust Sensor data
uint16_t Dust_Read(void)
{
  uint16_t Sensor_data;
  digitalWrite(pin_DUST_LED, LOW);
  delayMicroseconds(280);
  Sensor_data = analogRead(pin_DUST_OUT);
  delayMicroseconds(40);
  digitalWrite(pin_DUST_LED, HIGH);
  //delayMicroseconds(9680);
  return Sensor_data;
}

void TILT_ISR(void)
{
  Tilt_Value = !digitalRead(pin_TILT);
  Time_check_Tilt = millis();
}

void SOUND_ISR(void)
{
  if(Sound_pre != 1)
  {
    Sound_pre = 1;
    TX_Event_data[4] = 1;
    TX_Event_flag = 1;
    Sound_flag = 1;
  }
  Time_check_Sound = millis()+5000;
}

void PIR_ISR(void)
{
  if(PIR_pre != 1)
  {
    PIR_pre = 1;
    TX_Event_data[5] = 1;
    TX_Event_flag = 1;
    PIR_flag = 1;
  }
  Time_check_PIR = millis()+5000;
}

uint8_t SW_read(void)
{
  uint8_t i, da=0;
  for(i=0; i<2; i++)
  {
    da |= (digitalRead(pin_SW[i])<<i);
  }
  return da;
}

uint16_t ADC_TO_LUX(uint16_t ADC_data)  // CDS
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

uint16_t ADC_TO_PPM(uint16_t ADC_data)
{
  float ratio; 
  uint16_t ppm;
  ratio = ADC_data;
  ratio = ratio/(1023-ADC_data);  // Ro/Rs = Vo/Vs = ADC_data/(1023-ADC_data)
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

// ADC data -> [ug/m3]
float Dust_tr(uint16_t ADC_data)
{
  float Dust = ADC_data;
  Dust = Dust*5.0/1023.0/5.95 - 0.1;

  return Dust*1000;
}
