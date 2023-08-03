#ifndef _ProtocolPacket_XMT_H
#define _ProtocolPacket_XMT_H

#include <vector>
using namespace std;

typedef unsigned char BYTE;



class ProtocolPacket_XMT
{
public:
	BYTE Preamble = 0xAA;		//起始字节
	BYTE Address = 0;		//地址 0为广播(当地址为0时,全部下位机动作,设定地址时可用广播) 1～255为下位机地址
	BYTE Length = 0;		//包长
	BYTE B3 = 0;		//指令码1
	BYTE B4 = 0x02;		//指令码2 0x02代表命令
	BYTE Data[256];		//数据段
	BYTE Checksum = 0;		//校验和


	ProtocolPacket_XMT();
	ProtocolPacket_XMT(vector<BYTE> ArgByte);
	ProtocolPacket_XMT(BYTE nB3, vector<BYTE> ArgByte);


	vector<BYTE> build_protocol_packet(int nAddr, BYTE nLength, BYTE nB3, BYTE nB4, vector<BYTE> ArgByte);

	BYTE checksum();

	void show_protocol_packet();

	vector<BYTE> to_byte_array();
};
#endif

