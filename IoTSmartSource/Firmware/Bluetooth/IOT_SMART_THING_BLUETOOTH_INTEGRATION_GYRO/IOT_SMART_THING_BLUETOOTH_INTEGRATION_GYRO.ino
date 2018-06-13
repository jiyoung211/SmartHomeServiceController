#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>
#include <Wire.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23

#define GYRO 0x68

#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E

#define G_TO_READ 8 // 2 bytes for each temperature, axis x, y, z

uint8_t TX_Sensor_data[TX_Sensor_Packet_length] = {0x76, 0x30, };
uint8_t TX_Sensor_flag = 1, Gyro_Read = 0, Gyro_data_check = 0;
int32_t angle[3] = {0, 0, 0};
int32_t angle_offset[3] = {0, 0, 0};
int16_t gyro[3];

char Bluetooth_Name[] = "Arduino--";

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
//SPP SerialBT(&Btd); // This will set the name to the defaults: "Arduino" and the pin to "0000"
SPP SerialBT(&Btd, Bluetooth_Name, "1234"); // You can also set the name and pin like so

void setup() {
  // put your setup code here, to run once:
  uint8_t i, j;
  
  Serial.begin(115200);

  for(i=2; i<(TX_Sensor_Packet_length-1); i++)
  {
    TX_Sensor_data[i] = 0xFF;
  }
  TX_Sensor_data[TX_Sensor_Packet_length-1] = 0x3E;
  
  Wire.begin();
  initGyro();
  
  for(int i=0; i<50; i++)
  {
    getGyroscopeData(gyro);
    for(int j=0; j<3; j++)
    {
        angle_offset[j] = angle_offset[j] + gyro[j];
    }
  }

  for(int j=0; j<3; j++)
  {
      angle_offset[j] = angle_offset[j] / 50;
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
  uint8_t i;
  static int32_t angle_sum[3] = {0, 0, 0};
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    if(Gyro_Read == 1)
    {
      getGyroscopeData(gyro);
      for(i=0; i<3; i++)
      {
        angle_sum[i] = angle_sum[i] + (gyro[i] - angle_offset[i]);
      }

      if(Gyro_data_check == 1)
      {
        for(i=0; i<3; i++)
        {
          angle[i] = angle[i] + angle_sum[i]/14.375/20;
          angle_sum[i] = 0;
          while(angle[i] < 0)
            angle[i] = angle[i] + 360;
          while(angle[i] >= 360)
            angle[i] = angle[i] - 360;
        }
        Gyro_data_check = 0;
      }
      Gyro_Read = 0;
    }
    if(TX_Sensor_flag == 1)
    {
      TX_Sensor_data[16] = angle[0] >> 8;
      TX_Sensor_data[17] = angle[0] & 0xFF;
      TX_Sensor_data[18] = angle[1] >> 8;
      TX_Sensor_data[19] = angle[1] & 0xFF;
      TX_Sensor_data[20] = angle[2] >> 8;
      TX_Sensor_data[21] = angle[2] & 0xFF;

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
  static uint16_t Time_count_1s = 0, Time_count_50ms = 0;

  Gyro_Read = 1;

  Time_count_50ms++;
  if(Time_count_50ms == 20)
  {
    Time_count_50ms = 0;
    Gyro_data_check = 1;
    Time_count_1s++;
    if(Time_count_1s == 60)
    {
      TX_Sensor_flag = 1;
      Time_count_1s = 0;
    }
  }
}

void initGyro()
{
 /*****************************************
 * ITG 3200
 * power management set to:
 * clock select = internal oscillator
 * no reset, no sleep mode
 * no standby mode
 * sample rate to = 125Hz
 * parameter to +/- 2000 degrees/sec
 * low pass filter = 5Hz
 * no interrupt
 ******************************************/
  writeTo(GYRO, G_PWR_MGM, 0x00);
  writeTo(GYRO, G_SMPLRT_DIV, 0x07); // EB, 50, 80, 7F, DE, 23, 20, FF
  writeTo(GYRO, G_DLPF_FS, 0x1E); // +/- 2000 dgrs/sec, 1KHz, 1E, 19
  writeTo(GYRO, G_INT_CFG, 0x00);
}
void getGyroscopeData(int16_t * result)
{
  int16_t regAddress = 0x1D;
  byte buff[G_TO_READ];
  readFrom(GYRO, regAddress, G_TO_READ, buff); //read the gyro data from the ITG3200
  result[0] = ((buff[0] << 8) | buff[1]);
  result[1] = ((buff[2] << 8) | buff[3]);
  result[2] = ((buff[4] << 8) | buff[5]);
}

void writeTo(int DEVICE, byte address, byte val)
{
  Wire.beginTransmission(DEVICE); //start transmission to ACC 
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}

void readFrom(int DEVICE, byte address, int num, byte buff[]) 
{
  Wire.beginTransmission(DEVICE); //start transmission to ACC 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
 
  Wire.beginTransmission(DEVICE); //start transmission to ACC
  Wire.requestFrom(DEVICE, num);    // request 6 bytes from ACC
 
  int i = 0;
  while(Wire.available())    //ACC may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}
