#include <MsTimer2.h>
#include <Wire.h>

#define GYRO 0x68

#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E

uint8_t Gyro_Read = 0, Gyro_data_check = 0;
int32_t angle = 0;
int32_t angle_offset = 0;
int16_t gyro;

void setup() {
  uint8_t i;
  
  Serial.begin(115200);

  Wire.begin();
  initGyro();
  
  for(i=0; i<50; i++)
  {
    gyro = getGyroscopeDataY();
    angle_offset = angle_offset + gyro;
  }

  angle_offset = angle_offset / 50;
  
  MsTimer2::set(50, Sensor_Read_ISR);
  MsTimer2::start();
}

void loop() {
  static int32_t angle_sum = 0;
  if(Gyro_Read == 1)
  {
    gyro = getGyroscopeDataY();

    angle_sum = angle_sum + (gyro - angle_offset);

    if(Gyro_data_check == 1)
    {
      angle = angle + angle_sum/14.375/20;
      angle_sum = 0;
      while(angle < 0)
        angle = angle + 360;
      while(angle >= 360)
        angle = angle - 360;

      Serial.print("Y = ");
      Serial.println(angle);
      Gyro_data_check = 0;
    }
    Gyro_Read = 0;
  }
}

void Sensor_Read_ISR(void)
{
  static uint16_t Time_count_50ms = 0;

  Gyro_Read = 1;

  Time_count_50ms++;
  if(Time_count_50ms == 20)
  {
    Time_count_50ms = 0;
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

int16_t getGyroscopeDataX(void)
{
  uint8_t regAddress = 0x1D;
  byte buff[2];
  int16_t Gyro;
  readFrom(GYRO, regAddress, 2, buff); //read the gyro data from the ITG3200
  Gyro = ((buff[0] << 8) | buff[1]);
  return Gyro;
}

int16_t getGyroscopeDataY(void)
{
  uint8_t regAddress = 0x1F;
  byte buff[2];
  int16_t Gyro;
  readFrom(GYRO, regAddress, 2, buff); //read the gyro data from the ITG3200
  Gyro = ((buff[0] << 8) | buff[1]);
  return Gyro;
}

int16_t getGyroscopeDataZ(void)
{
  uint8_t regAddress = 0x21;
  byte buff[2];
  int16_t Gyro;
  readFrom(GYRO, regAddress, 2, buff); //read the gyro data from the ITG3200
  Gyro = ((buff[0] << 8) | buff[1]);
  return Gyro;
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
