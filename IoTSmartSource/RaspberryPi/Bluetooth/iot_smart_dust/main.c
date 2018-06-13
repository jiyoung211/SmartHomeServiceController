#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,dust_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		dust_val = series_sensor_read(DUST_C);
		sleep(10); // sleep is 10s
		printf("dust-> %d ug/m^3 \n",dust_val);
	}
	bt_release(bt_sockt);
	return 0;
}
