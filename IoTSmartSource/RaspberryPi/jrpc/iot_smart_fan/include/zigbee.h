#ifndef __ZIGBEE_H__
#define __ZIGBEE_H__

#define BUFFER_MAX 64
#define SERIAL_DEV_NAME		"/dev/ttyUSB0"
#define SERIAL_SPEED		57600
#define CMD_FAN_ON 			0x0E
#define CMD_FAN_OFF 		0x0D
#define MODULE_ID 			0x5E

typedef struct _tx_packet_header
{
	unsigned char frame_type;
	unsigned char seq;
	unsigned char frame_dispatch;
	unsigned short dest_addr;
	unsigned short src_addr;
	unsigned char payload_length;
	unsigned char group_id;
	unsigned char type;
}__attribute__((packed)) tx_packet_header_t;

typedef struct _user_data
{
	unsigned char module_id;
	unsigned char cmd;
	unsigned char subcmd;
	unsigned char data_length;
}__attribute__((packed)) user_data_t;

typedef struct _ctrl_packet
{
	tx_packet_header_t packet_header;
	user_data_t user_data;
	unsigned short crc;	
}__attribute__((packed)) ctrl_packet_t;

unsigned short crcByte(unsigned char *data, unsigned char Length);
unsigned char send_command(unsigned char cmd, unsigned char subcmd);

#endif