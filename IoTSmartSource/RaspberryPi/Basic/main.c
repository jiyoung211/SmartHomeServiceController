#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,select,flag=1;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	

	bt_release(bt_sockt);
	return 0;
}
