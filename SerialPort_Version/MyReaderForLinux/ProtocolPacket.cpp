#include "ProtocolPacket.h"
#include <vector>
#include <iostream>

using namespace std;

ProtocolPacket::ProtocolPacket()
{

}

ProtocolPacket::~ProtocolPacket()
{

}

ProtocolPacket::ProtocolPacket(vector<unsigned char> ArgByte)
{
	Append(ArgByte);
}

ProtocolPacket::ProtocolPacket(unsigned char nCode, unsigned char nType)
{
	vector<unsigned char> emptyVector;
	BuildCmdPacketByte(2, 65535, nCode, nType, emptyVector);
}

ProtocolPacket::ProtocolPacket(unsigned char nCode, unsigned char nType, vector<unsigned char> ArgByte)
{
	BuildCmdPacketByte(2, 65535, nCode, nType, ArgByte);
}

ProtocolPacket::ProtocolPacket(int nAddr, unsigned char nCode, unsigned char nType)
{
	vector<unsigned char> emptyVector;
	BuildCmdPacketByte(2, nAddr, nCode, nType, emptyVector);
}

ProtocolPacket::ProtocolPacket(int nAddr, unsigned char nCode, unsigned char nType, vector<unsigned char> ArgByte)
{
	BuildCmdPacketByte(2, nAddr, nCode, nType, ArgByte);
}

ProtocolPacket::ProtocolPacket(unsigned char nHead, int nAddr, unsigned char nCode, unsigned char nType, vector<unsigned char> ArgByte)
{
	BuildCmdPacketByte(nHead, nAddr, nCode, nType, ArgByte);
}

void ProtocolPacket::Append(vector<unsigned char> ArgByte)
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

vector<unsigned char> ProtocolPacket::BuildCmdPacketByte(unsigned char nHead, int nAddr, unsigned char nCode, unsigned char nType, vector<unsigned char> ArgByte)
{
	this->Preamble = nHead;
	this->Address[0] = (unsigned char)nAddr;
	this->Address[1] = (unsigned char)(nAddr >> 8);
	this->Code = nCode;
	this->Type = nType;
	this->Length = ArgByte.size();
	for (int i = 0; i < Length; i++)
		this->PayLoad[i] = ArgByte[i];
	this->Checksum = CheckSum();

	return ToVectorArray();
}

unsigned char ProtocolPacket::CheckSum()
{
	unsigned char checksum = 0;
	checksum = (unsigned char)(checksum + this->Preamble);
	checksum = (unsigned char)(checksum + this->Address[0]);
	checksum = (unsigned char)(checksum + this->Address[1]);
	checksum = (unsigned char)(checksum + this->Code);
	checksum = (unsigned char)(checksum + this->Type);
	checksum = (unsigned char)(checksum + this->Length);
	for (int i = 0; i < Length; i++)
		checksum = (unsigned char)(checksum + this->PayLoad[i]);
	return (unsigned char)(~checksum + 1 & 0xFF);
}

vector<unsigned char> ProtocolPacket::ToVectorArray()
{
	vector<unsigned char> array;
	array.push_back(this->Preamble);
	array.push_back(this->Address[0]);
	array.push_back(this->Address[1]);
	array.push_back(this->Code);
	array.push_back(this->Type);
	array.push_back(this->Length);
	if (this->Code == 17 && this->PayLoad[0] == 14)		//不懂
	{
		for (int i = 0; i < Length * 14 + 1; i++)
		{
			try
			{
				array.push_back(this->PayLoad[i]);
			}
			catch (const char* exception)
			{
				cout << "Exception:" << exception << endl;
			}
		}
	}
	else
	{
		for (int j = 0; j < Length; j++)
		{
			try
			{
				array.push_back(this->PayLoad[j]);
			}
			catch (const char* exception)
			{
				cout << "Exception:" << exception << endl;
			}
		}

		array.push_back(this->Checksum);
	}

	return array;
}

void ProtocolPacket::showProtocolPacket()
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
				cout << "标签(EPC) :";
				for (int i = 3; i < Length - 1; i++)
					printf("%02X ", PayLoad[i]);
				printf("\n");
			}
			break;
		case 34:
			if (Type == 0)	//PayLoad[i]为ANT(1), PC(2), EPC(12)， DATA(不定)
			{
				int i;
				cout << "标签(EPC) :";
				for (i = 0; i < EPCLength; i++)
					printf("%02X ", PayLoad[i + 3]);
				printf("\n");

				if (Length - EPCLength > 3)
				{
					cout << "数据(Data):";
					for (i = 0; i < Length - EPCLength - 3; i++)
						printf("%02X ", PayLoad[i + 3 + EPCLength]);
					printf("\n");
				}
			}
			break;
		case 35:
			if (Type == 0) //PayLoad[i]为ANT(1), PC(2), EPC(12)
			{
				cout << "写入标签(EPC)";
				for (int i = 0; i < EPCLength; i++)
					printf("%02X ", PayLoad[i + 3]);
				cout << "成功 :)" << endl;
			}
			break;
		default:
			break;
		}
	}
}