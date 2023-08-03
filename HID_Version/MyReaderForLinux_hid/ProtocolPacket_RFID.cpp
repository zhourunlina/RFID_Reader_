#include "ProtocolPacket_RFID.h"
#include <vector>
#include <iostream>
using namespace std;

ProtocolPacket_RFID::ProtocolPacket_RFID(){}

ProtocolPacket_RFID::ProtocolPacket_RFID(vector<BYTE> ArgByte)
{
	append(ArgByte);
}

ProtocolPacket_RFID::ProtocolPacket_RFID(int nAddr, BYTE nCode, BYTE nType)
{
	vector<BYTE> emptyVector;
	build_cmd_byte_array(2, nAddr, nCode, nType, emptyVector);
}

ProtocolPacket_RFID::ProtocolPacket_RFID(int nAddr, unsigned char nCode, unsigned char nType, vector<unsigned char> ArgByte)
{
	build_cmd_byte_array(2, nAddr, nCode, nType, ArgByte);
}

ProtocolPacket_RFID::~ProtocolPacket_RFID(){}

void ProtocolPacket_RFID::append(vector<BYTE> ArgByte)
{
	int i = 0;
	int num = ArgByte.size();
	try
	{
		this->Preamble = ArgByte[i++];
		this->Address[0] = ArgByte[i++];
		this->Address[1] = ArgByte[i++];
		this->Code = ArgByte[i++];
		this->Type = ArgByte[i++];
		this->Length = ArgByte[i++];
		this->Checksum = ArgByte[i + Length];
		for (; i < ((num > 255) ? 255 : num); i++)
		{
			this->PayLoad[i - 6] = ArgByte[i];
		}
	}
	catch (exception ex)
	{
		throw ex;
	}
}


vector<BYTE> ProtocolPacket_RFID::build_cmd_byte_array(BYTE nHead, int nAddr, BYTE nCode, BYTE nType, vector<BYTE> ArgByte)
{
	this->Preamble = nHead;
	this->Address[0] = (BYTE)nAddr;
	this->Address[1] = (BYTE)(nAddr >> 8);
	this->Code = nCode;
	this->Type = nType;
	this->Length = ArgByte.size();
	for (int i = 0; i < Length; i++)
		this->PayLoad[i] = ArgByte[i];
	this->Checksum = checksum();

	return to_byte_array();
}

BYTE ProtocolPacket_RFID::checksum()
{
	BYTE checksum = 0;
	checksum = (BYTE)(checksum + this->Preamble);
	checksum = (BYTE)(checksum + this->Address[0]);
	checksum = (BYTE)(checksum + this->Address[1]);
	checksum = (BYTE)(checksum + this->Code);
	checksum = (BYTE)(checksum + this->Type);
	checksum = (BYTE)(checksum + this->Length);
	for (int i = 0; i < Length; i++)
		checksum = (BYTE)(checksum + this->PayLoad[i]);
	return (BYTE)(~checksum + 1 & 0xFF);
}


void ProtocolPacket_RFID::show_protocol_packet()
{
	if (this->Preamble == 2)
	{
		printf("CMD : ");
		printf("%02X %02X %02X %02X %02X %02X ", this->Preamble, this->Address[0], this->Address[1],
			this->Code, this->Type, this->Length);
		for (int i = 0; i < Length; i++)
			printf("%02X ", PayLoad[i]);
		printf("%02X ", Checksum);
		printf("\n");
	}

	if (this->Preamble == 3)
	{
		printf("RSP : ");
		printf("%02X %02X %02X %02X %02X %02X ", this->Preamble, this->Address[0], this->Address[1],
			this->Code, this->Type, this->Length);
		for (int i = 0; i < Length; i++)
			printf("%02X ", PayLoad[i]);
		printf("%02X ", Checksum);
		printf("\n");

		switch (this->Code)
		{
		case 33:
			if (Type == 2)	//PayLoad[i]为ANT(1), PC(2), EPC(12)， RSSI(1)
			{
				cout << "标签(EPC):";
				for (int i = 3; i < Length - 1; i++)
					printf("%02X ", PayLoad[i]);
				printf("\n");
			}
			break;
		case 34:
			if (Type == 0)	//PayLoad[i]为ANT(1), PC(2), EPC(12)， DATA(不定)
			{
				int i;
				cout << "标签(EPC):";
				for (i = 0; i < 12; i++)					//标签长度为12
					printf("%02X ", PayLoad[i + 3]);
				printf("\n");

				if (Length - 12 > 3)
				{
					cout << "数据(Data):";
					for (i = 0; i < Length - 12 - 3; i++)
						printf("%02X ", PayLoad[i + 3 + 12]);
					printf("\n");
				}
			}
			break;
		case 35:
			if (Type == 0) //PayLoad[i]为ANT(1), PC(2), EPC(12)
			{
				cout << "写入标签(EPC)";
				for (int i = 0; i < 12; i++)
					printf("%02X ", PayLoad[i + 3]);
				cout << "成功 :)" << endl;
			}
			break;
		default:
			break;
		}
	}
}

vector<BYTE> ProtocolPacket_RFID::to_byte_array()
{
	vector<BYTE> array;
	array.push_back(this->Preamble);
	array.push_back(this->Address[0]);
	array.push_back(this->Address[1]);
	array.push_back(this->Code);
	array.push_back(this->Type);
	array.push_back(this->Length);

	for (int j = 0; j < Length; j++)	array.push_back(this->PayLoad[j]);

	array.push_back(this->Checksum);

	return array;
}
