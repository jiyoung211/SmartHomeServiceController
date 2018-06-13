#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CS_MCP3208 29			//SPI Interface CS Pin
#define SPI_CHANNEL 0			//SPI Channel
#define SPI_SPEED   1000000		//SPI Speed
#define ADC_CHANNEL	0

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
	int adc_gas = 0;
	float gas_persent = 0;
	
	printf("Raspberry Pi Gas Sensor Test !! \n");

	if (wiringPiSetup() == -1)
	{
		fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
		return 0;
	}

	if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
	{
		fprintf(stdout, "SPISetup Failed: %s\n", strerror(errno));
		return 0;
	}

	pinMode(CS_MCP3208, OUTPUT);				//setting Pinmode  

	while (1)
	{
		adc_gas = analogReadPI(ADC_CHANNEL);	
		gas_persent = (float)adc_gas / 4096 * 100;
		
		printf("Gas Value : %0.2f%\n", gas_persent);

		delay(1000);
	}

	return 0;
}

