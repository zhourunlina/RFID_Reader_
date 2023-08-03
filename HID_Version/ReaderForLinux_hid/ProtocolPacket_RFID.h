#ifndef _ProtocolPacket_H
#define _ProtocolPacket_H

#include <vector>
using namespace std;

typedef unsigned char BYTE;

const BYTE MSG_CMD = 00;				//0x00
const BYTE MSG_GET = 50;				//0x32

const BYTE READ_EPC_ONLY = 33;			//0x21
const BYTE READ_EPC_AND_DATA = 34;		//0x22
const BYTE WRITE_DATA = 35;				//0x23
const BYTE GET_INFO = 131;				//0x83


class ProtocolPacket_RFID
{
public:
	BYTE Preamble = 2;		//起始位
	BYTE Address[2] = { 0xFF,0xFF };		//地址
	BYTE Code = 0;		//CID1
	BYTE Type = 0;		//CID2 或 RTN
	BYTE Length = 0;	//INFO长度
	BYTE PayLoad[1024];	//INFO，即Data
	BYTE Checksum = 0;		//校验和，其他位求和模256余数取反+1

	ProtocolPacket_RFID();
	ProtocolPacket_RFID(vector<BYTE> ArgByte);
	ProtocolPacket_RFID(int nAddr, BYTE nCode, BYTE nType);
	ProtocolPacket_RFID(int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte);
	~ProtocolPacket_RFID();

	void append(vector<BYTE> ArgByte);

	vector<BYTE> build_cmd_byte_array(BYTE nHead, int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte);

	BYTE checksum();

	void show_protocol_packet();

	vector<BYTE> to_byte_array();
};
#endif