#include <MsTimer2.h>
#include <Wire.h>

#define GYRO 0x68

#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E

#define G_TO_READ 6 // 2 bytes for axis x, y, z

uint8_t Gyro_Read = 0, Gyro_data_check = 0;
int32_t angle[3] = {0, 0, 0};
int32_t angle_offset[3] = {0, 0, 0};
int16_t gyro[3];

void setup() {
  uint8_t i, j;
  
  Serial.begin(115200);

  Wire.begin();
  initGyro();
  
  for(i=0; i<50; i++)
  {
    getGyroscopeData(gyro);
    for(j=0; j<3; j++)
    {
      angle_offset[j] += gyro[j];
    }
  }

  for(int j=0; j<3; j++)
  {
    angle_offset[j] /= 50;
  }
  
  MsTimer2::set(50, Sensor_Read_ISR);
  MsTimer2::start();
}

void loop() {
  uint8_t i;
  static int32_t angle_sum[3] = {0, 0, 0};
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
      Serial.print("X = ");
      Serial.print(angle[0]);
      Serial.print(", Y = ");
      Serial.print(angle[1]);
      Serial.print(", Z= ");
      Serial.println(angle[2]);
      Gyro_data_check = 0;
    }
    Gyro_Read = 0;
  }
}

void Sensor_Read_ISR(void)
{
  static uint16_t Time_count_1s = 0;

  Gyro_Read = 1;

  Time_count_1s++;
  if(Time_count_1s == 20)
  {
    Time_count_1s = 0;
    Gyro_data_check = 1;
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
  uint8_t regAddress = 0x1D;
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
