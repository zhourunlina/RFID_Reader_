#ifndef _ProtocolPacket_H
#define _ProtocolPacket_H

#include <vector>
using namespace std;

typedef unsigned char BYTE;

class ProtocolPacket
{
public:
	const int EPCLength = 12;

	BYTE Preamble = 2;		//起始位
	BYTE Address[2] = { 0xFF,0xFF };		//地址
	BYTE Code = 0;		//CID1
	BYTE Type = 0;		//CID2 或 RTN
	BYTE Length = 0;	//INFO长度
	BYTE PayLoad[1024];	//INFO，即Data
	BYTE Checksum = 0;		//校验和，其他位求和模256余数取反+1

	ProtocolPacket();
	ProtocolPacket(vector<BYTE> ArgByte);
	ProtocolPacket(BYTE nCode, BYTE nType);
	ProtocolPacket(BYTE nCode, BYTE nType, vector<BYTE> ArgByte);
	ProtocolPacket(int nAddr, BYTE nCode, BYTE nType);
	ProtocolPacket(int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte);
	ProtocolPacket(BYTE nHead, int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte);
	~ProtocolPacket();

	void Append(vector<BYTE> ArgByte);

	vector<BYTE> BuildCmdPacketByte(BYTE nHead, int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte);

	BYTE CheckSum();

	vector<BYTE> ToVectorArray();

	void showProtocolPacket();
};

#endif // !_ProtocolPacket_H
