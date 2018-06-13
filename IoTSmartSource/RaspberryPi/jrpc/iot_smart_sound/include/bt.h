#ifndef __BT_H__
#define __BT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

//#define DEBUG

#ifdef DEBUG
#define DBG(fmt, args...)       printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define STX     0x76
#define ETX     0x3E

#define BT_MAC "00:15:83:E7:2B:B6"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// ADK send & recv struct
typedef struct {
    uchar stx;
    uchar cmd;
    uchar led;
    uchar dc_motor;
    uchar step_motor;
    uchar servo_motor;
    uchar fnd_H;
    uchar fnd_L;
    uchar etx;
} actuator_t;

// Event Sensor packet structure
typedef struct {
    uchar stx;
    uchar cmd;
    uchar button;
    uchar tilt;
    uchar sound;
    uchar pir;
    uchar etx;
} event_sensor_t;

// Series Sensor packet stureture
typedef struct {
    uchar stx;
    uchar cmd;
    uchar pulse;
    uchar temp;
    uchar humi;
    uchar psd;
    uchar ir;
    uchar cds;
    uchar rain;
    uchar soil;
    uchar dust_H;
    uchar dust_L; 
    uchar gas_H;
    uchar gas_L;  
    uchar vr_H;
    uchar vr_L;
    uchar gyro_X_H;
    uchar gyro_X_L;
    uchar gyro_Y_H;
    uchar gyro_Y_L;
    uchar gyro_Z_H;
    uchar gyro_Z_L;
    uchar etx;
} series_sensor_t;

//data packet structure to store the returned data
typedef struct {
	int pulse;
	int temp;
	int humi;
	int psd;
	int ir;
	int cds;
	int gas;
	int dust;
	int rain;
	int soil;
	int vr;
	int gyro_x;
	int gyro_y;
	int gyro_z;
} series_sensor_data_t;

//bluetooth socket function ..
int act_send(int arg);
int event_sensor_read(int arg);
int series_sensor_read(int arg);
int bt_init(char *bt_name);
void bt_config(int socket);
int event_flag();

//bluetooth value
actuator_t actuator_packet;
event_sensor_t event_sensor_packet;
event_sensor_t event_sensor_data;
series_sensor_t series_sensor_packet;
series_sensor_data_t series_sensor_data;

#endif /* __BT_H__ */
