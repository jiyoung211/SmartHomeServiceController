#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,tilt_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		tilt_val = event_sensor_read(TILT_C);  //bluetooth pars data read 
		sleep(1);  //sleep is 1s
		printf("tilt -> %d \n",tilt_val);
	}
	bt_release(bt_sockt);
	return 0;
}
