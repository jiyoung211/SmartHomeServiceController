#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		set_DC(2);	// DC speed 2 0 ~ 5
		printf("DC ON !\n");
		sleep(2); // sleep is 2s
		set_DC(0);	
		printf("DC OFF !\n");
		sleep(2); // sleep is 2s
	}
	bt_release(bt_sockt);
	return 0;
}
