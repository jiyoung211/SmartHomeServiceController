#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

//SHT20 I2C
#define SHT20_I2C_ADDR             0x40
#define SHT20_SOFT_RESET           0xFE
#define SHT20_I2C_CMD_MEASURE_TEMP 0xF3
#define SHT20_I2C_CMD_MEASURE_HUMI 0xF5

//CDS SPI
#define CS_MCP3208 29			//SPI Interface CS Pin
#define SPI_CHANNEL 0			//SPI Channel
#define SPI_SPEED   1000000		//SPI Speed
#define ADC_CHANNEL 0


int analogReadPI(unsigned char adcChannel)
{
	unsigned char buff[3];
	int adcValue = 0;

	//setting communication packet
	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;

	digitalWrite(CS_MCP3208, 0);	//start SPI(LOW Active)

	wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);	//read data

	buff[1] = 0x0F & buff[1];
	adcValue = (buff[1] << 8) | buff[2];

	digitalWrite(CS_MCP3208, 1);	//stop SPI

	return adcValue;
}


int main(void)
{
	float temp = 0.0; // temperature
	float humi = 0.0; // releative humidity
	unsigned char data[3];
	int i;
	int val;
	int adc_cds = 0;
	int devSHT20 = -1;

	printf("Raspberry Pi SHT20 and CdS Sensor Test !! \n");

	if (wiringPiSetup() == -1)
	{
		fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
		return 0;
	}

	if ((devSHT20 = wiringPiI2CSetup(SHT20_I2C_ADDR)) == -1)
	{
		fprintf(stderr, "SHT20 : Unable to initialise I2C: %s\n", strerror(errno));
		return 0;
	}

	if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
	{
		fprintf(stdout, "SPISetup Failed: %s\n", strerror(errno));
		return 0;
	}

	pinMode(CS_MCP3208, OUTPUT);

	wiringPiI2CWrite(devSHT20, SHT20_SOFT_RESET);
	delay(50);

	while (1)
	{
		//=== Temperature ================================================
		wiringPiI2CWrite(devSHT20, SHT20_I2C_CMD_MEASURE_TEMP);
		delay(260);

		for (i = 0; i < 2; i++)
			data[i] = wiringPiI2CRead(devSHT20);

		val = data[0] << 8 | data[1];
		temp = -46.85 + 175.72 / 65536 * (int)val;

		//=== Humidity ===================================================
		wiringPiI2CWrite(devSHT20, SHT20_I2C_CMD_MEASURE_HUMI);
		delay(60);

		for (i = 0; i < 2; i++)
			data[i] = wiringPiI2CRead(devSHT20);

		val = data[0] << 8 | data[1];
		humi = -6.0 + 125.0 / 65536 * (int)val;

		//cds
		adc_cds = analogReadPI(ADC_CHANNEL);

		//=== Command Print ==============================================
		printf("Temperature : %.1f / Humidity : %.0f / Cds : %d\n", (float)temp, (float)humi, adc_cds);

		delay(1000);
	}
	return 0;
}

