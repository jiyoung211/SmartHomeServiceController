#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,pulse_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		pulse_val = series_sensor_read(PULSE_C);  //bluetooth pars data read 
		sleep(10); // sleep is 10s 
		printf("pulse -> %d \n",pulse_val);
	}
	bt_release(bt_sockt);
	return 0;
}
