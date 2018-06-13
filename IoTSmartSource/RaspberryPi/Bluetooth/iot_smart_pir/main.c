#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,pir_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		pir_val = event_sensor_read(PIR_C);  //bluetooth pars data read 
		sleep(1);  // sleep is 1s 
		printf("pir -> %d \n",pir_val);
	}
	bt_release(bt_sockt);
	return 0;
}
