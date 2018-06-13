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

#define PULSE_QUE_SIZE 5
int main(void)
{
	int idx = 0;
	char is_falling = -1;
	int pre_adc = 0;
	int cur_adc = 0;

	int adc_min = 9999;
	int cur_max = -9999;
	
	int start_idx = -1, last_idx = -1;
	int pulse_count = 0;

	float pulse_rate[PULSE_QUE_SIZE] = { 0, };
	int que_idx = 0;
	int que_count = 0;
	float avg_pulse_rate = 0;
	float cur_pulse_rate = 0;


	printf("Raspberry Pi Pulse Sensor Test !! \n");

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

	printf("wait 3 sec .. measure vpp !! \n");
	for (idx = 0; idx < 150; idx++)
	{
		cur_adc = analogReadPI(ADC_CHANNEL);
		if (cur_adc)
		{
			if (adc_min > cur_adc)
				adc_min = cur_adc;
			if (cur_max < cur_adc)
				cur_max = cur_adc;
		}
		delay(20);
	}

	printf("start !! - initial max:%d min:%d\n", adc_min, cur_max);
	pre_adc = cur_adc;

	idx = 0;
	while (1)
	{
		if (idx == 250)
		{
			if (pulse_count)
			{
				cur_pulse_rate = (float)(pulse_count - 1) * 60 * 1000 / ((last_idx - start_idx) * 20);
				pulse_rate[que_idx++%PULSE_QUE_SIZE] = cur_pulse_rate;
				if (que_count < PULSE_QUE_SIZE)
					que_count++;
			}
			avg_pulse_rate = 0;
			for (idx = 0; idx < que_count; idx++)
				avg_pulse_rate += pulse_rate[idx];
			avg_pulse_rate /= que_count;
			printf("\npulse : %0.2f\n", avg_pulse_rate);

			pulse_count = 0;
			idx = 0;
			start_idx = -1;
			last_idx = -1;
		}

		cur_adc = analogReadPI(ADC_CHANNEL);

		if (pre_adc > cur_adc)
		{

			if (is_falling == 0)
			{
				if (pre_adc >= adc_min + (cur_max - adc_min)*0.6)
				{
					pulse_count++;
					printf("#");
					fflush(stdout);
					if (start_idx == -1)
						start_idx = idx;

					last_idx = idx;
				}
			}

			is_falling = 1;
		}
		else if (pre_adc < cur_adc)
		{
			is_falling = 0;
		}

		//printf("adc prev:%d cur:%d\n", pre_adc , cur_adc);
		pre_adc = cur_adc;
		delay(20);
		idx++;

	}

	return 0;
}

