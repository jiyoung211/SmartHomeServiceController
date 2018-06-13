#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>

#define Group_ID  10

#define TX_Event_Packet_length 7
#define RX_Packet_length 9

int pin_LED[2] = {22, 23};
int pin_SW[2] = {24, 25};

uint8_t TX_Event_data[TX_Event_Packet_length] = {0x76, 0x31, };
uint8_t TX_Event_flag = 0;
uint8_t RX_data[RX_Packet_length];
uint8_t SW_pre = 0;
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

  for(i=2; i<(TX_Event_Packet_length-1); i++)
  {
    TX_Event_data[i] = 0xFF;
  }
  TX_Event_data[TX_Event_Packet_length-1] = 0x3E;

  for(i=0; i<2; i++)
  {
    pinMode(pin_SW[i], INPUT);
    pinMode(pin_LED[i], OUTPUT);
    digitalWrite(pin_LED[i], LOW);
  }

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
  uint8_t i;
  char da = 0;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    // receive data check
    if(SerialBT.available() > 0)
    {
      da = SerialBT.read();

      // receive data check
      if((rx_count == 0) && (da == 0x76))
      {
        RX_data[rx_count] = da;
        rx_count++;
      }
      else if(rx_count == 1)
      {
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
          // checksum
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
          }
        }
      }
    }
    if(TX_Event_flag == 1)
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
  }
}

void Sensor_Read_ISR(void)
{
  uint8_t da = SW_read();
  if(SW_pre != da)
  {
    SW_pre = da;
    TX_Event_data[2] = da;
    TX_Event_flag = 1;
  }
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


