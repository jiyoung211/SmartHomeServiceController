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
	int adc_sound = 0;
	int count = 0;

	printf("Raspberry Pi Sound Sensor Test !! \n");

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
		adc_sound = analogReadPI(ADC_CHANNEL);			//read adc0 value
		
		if (adc_sound >= 430)
		{
			printf("SOUND Detection : YES, %d\n", count++);
		}
		delay(100);
	}

	return 0;
}

