#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,x_val,y_val,z_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		x_val = series_sensor_read(GYROSCOPE_X_C);
		y_val = series_sensor_read(GYROSCOPE_Y_C);
		z_val = series_sensor_read(GYROSCOPE_Z_C);
		printf("x -> %d , y -> %d , z -> %d\n",x_val,y_val,z_val);
		sleep(1);
	}
	bt_release(bt_sockt);
	return 0;
}
