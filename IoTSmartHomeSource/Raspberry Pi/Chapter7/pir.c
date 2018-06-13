#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>

#define PIR_INPUT 2

int main (void)
{
        int detection = 0;
        unsigned int i = 0;
        printf ("Raspberry Pi PIR Sensor Test !! \n") ;

        if (wiringPiSetup() == -1)
        {
                fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
                return 0;
        }

        pinMode (PIR_INPUT, INPUT) ;

        for (i=0;;i++)
        {
                detection = digitalRead (PIR_INPUT);
                if(detection)
                        printf("[%d]Detection : Yes\n", i);
                else
                        printf("[%d]Detection : No\n", i);
                delay (1000) ;
        }
        return 0 ;
}
