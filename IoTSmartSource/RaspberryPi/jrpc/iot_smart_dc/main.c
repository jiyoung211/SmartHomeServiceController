#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,actFlg, dcControl;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();

	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

