#include "zigbee.h"
#include "main.h"
#include "bt.h"

int serial_fd = -1;

unsigned short crcByte(unsigned char *data, unsigned char Length)
{
	unsigned short crc = 0, i;
	for (i = 0; i < Length; i++)
	{
		crc = (unsigned char)(crc >> 8) | (crc << 8);
		crc ^= data[i];
		crc ^= (unsigned char)(crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= (crc & 0xff) << 5;
	}
	return crc;
}

unsigned char send_command(unsigned char cmd, unsigned char subcmd)
{
	int idx = 0, tx_idx = 0;
	int writeRsult, readRsult;
	unsigned char tx_data[BUFFER_MAX] = {0,};
	ctrl_packet_t packet = { 0, };
	unsigned char* ptemp = NULL;
	
	serial_fd = serialOpen(SERIAL_DEV_NAME, SERIAL_SPEED);	
	
	packet.packet_header.frame_type = 0x44;
	packet.packet_header.seq = 00;
	packet.packet_header.frame_dispatch = 0x00;
	packet.packet_header.dest_addr = 0xffff;
	packet.packet_header.src_addr = 0xffff;
	packet.packet_header.payload_length = sizeof(user_data_t);
	packet.packet_header.group_id = 00;
	packet.packet_header.type = 0x40;

	packet.user_data.module_id = MODULE_ID;
	packet.user_data.cmd = cmd;
	packet.user_data.subcmd = subcmd;
	packet.user_data.data_length = 0;

	packet.crc = crcByte((unsigned char*)&packet.packet_header, sizeof(tx_packet_header_t) + sizeof(user_data_t));
	
	tx_data[tx_idx++] = 0x7e;	//start
	ptemp = (unsigned char*)&packet;
	for(idx = 0; idx < sizeof(packet) && tx_idx < BUFFER_MAX-1; idx++)
	{
		if(ptemp[idx] == 0x7e || ptemp[idx] == 0x7d)
		{
			tx_data[tx_idx++] = 0x7d;
			tx_data[tx_idx++] = ptemp[idx]^0x20;
		}
		else
		{
			tx_data[tx_idx++] = ptemp[idx];
		}
	}	
	tx_data[tx_idx++]  = 0x7e; //end
	
	for (idx = 0; idx < tx_idx; idx++)
	{
		serialPutchar(serial_fd, tx_data[idx]);
		delayMicroseconds(1);
	}
	
	serialClose(serial_fd);
	
	return 0;
}