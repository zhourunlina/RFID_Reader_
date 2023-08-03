#include "ProtocolPacket_XMT.h"
#include <vector>
#include <iostream>
using namespace std;

ProtocolPacket_XMT::ProtocolPacket_XMT() {}


ProtocolPacket_XMT::ProtocolPacket_XMT(BYTE nB3, vector<BYTE> ArgByte)
{
	build_protocol_packet(0x01, ArgByte.size() + 6, nB3, 0x02, ArgByte);
}


//处理发送的包
vector<BYTE> ProtocolPacket_XMT::build_protocol_packet(int nAddr, BYTE nLength, BYTE nB3, BYTE nB4, vector<BYTE> ArgByte)//最后一个参数为数据段
{
	Address = nAddr;
	Length = nLength;
	B3 = nB3;
	B4 = nB4;
	for (int i = 0; i < ArgByte.size(); i++)
	{
		Data[i] = ArgByte[i];
	}
	Checksum = checksum();

	return to_byte_array();
}

BYTE ProtocolPacket_XMT::checksum()//计算校验和
{
	BYTE checksum = 0;
	checksum ^= Preamble;
	checksum ^= Address;
	checksum ^= Length;
	checksum ^= B3;
	checksum ^= B4;
	for (int i = 0; i < Length - 6; i++)
	{
		checksum = checksum ^ Data[i];
	}
	return checksum;
}


void ProtocolPacket_XMT::show_protocol_packet()//显示包内容
{
	printf("%02X %02X %02X %02X %02X ", Preamble, Address, Length, B3, B4);
	for (int i = 0; i < Length - 6; i++)
	{
		printf("%02X ", Data[i]);
	}
	printf("%02X\n", Checksum);
}

vector<BYTE> ProtocolPacket_XMT::to_byte_array()//拼接包
{
	vector<BYTE> array;
	array.push_back(Preamble);
	array.push_back(Address);
	array.push_back(Length);
	array.push_back(B3);
	array.push_back(B4);
	for (int i = 0; i < Length - 6; i++)
	{
		array.push_back(Data[i]);
	}
	array.push_back(Checksum);
	return array;
}
