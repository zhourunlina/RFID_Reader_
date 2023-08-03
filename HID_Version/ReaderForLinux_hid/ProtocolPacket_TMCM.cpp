#include "ProtocolPacket_TMCM.h"
#include <iostream>
using namespace std;

ProtocolPacket_TMCM::ProtocolPacket_TMCM() {}

ProtocolPacket_TMCM::ProtocolPacket_TMCM(int command)
{
	vector<BYTE> args;
	for (int i = 0; i < 4; i++) args.push_back(0x00);

	build_protocol_packet(0x01, command, 0x00, 0x00, args);
}



ProtocolPacket_TMCM::ProtocolPacket_TMCM(int command, int type,int value)
{
	vector<BYTE> args;
	args.push_back((BYTE)(value >> 24));
	args.push_back((BYTE)(value >> 16));
	args.push_back((BYTE)(value >> 8));
	args.push_back((BYTE)value);

	build_protocol_packet(0x01, command, type, 0x00, args);

}

ProtocolPacket_TMCM::ProtocolPacket_TMCM(int command, int type, int motor,int value)
{
    vector<BYTE> args;
    args.push_back((BYTE)(value >> 24));
    args.push_back((BYTE)(value >> 16));
    args.push_back((BYTE)(value >> 8));
    args.push_back((BYTE)value);

    build_protocol_packet(0x01, command, type, motor, args);
}

ProtocolPacket_TMCM::~ProtocolPacket_TMCM() {}

BYTE ProtocolPacket_TMCM::checksum()
{
	BYTE checksum = 0;
	checksum += this->Address;
	checksum += this->Command;
	checksum += this->Type;
	checksum += this->Motor;
	for (int i = 0; i < 4; i++) checksum += Data[i];

	return checksum;
}

void ProtocolPacket_TMCM::build_protocol_packet(BYTE nAddr, BYTE nCommand, BYTE nType, BYTE nMotor, vector<BYTE> ArgByte)
{
	this->Address = nAddr;
	this->Command = nCommand;
	this->Type = nType;
	this->Motor = nMotor;
	for (int i = 0; i < 4; i++) this->Data[i] = ArgByte[i];		//上位机对下位机的命令Data = 4
	this->Checksum = checksum();
}

void ProtocolPacket_TMCM::show_protocol_packet()
{
//    if( this->Command == TMCL_GIO ) return;
    cout << this->Command << endl;
	printf("%02X %02X %02X %02X ", this->Address, this->Command, this->Type, this->Motor);
	for (int i = 0; i < 4; i++) printf("%02X ", this->Data[i]);
	printf("%02X\n", this->Checksum);
}

vector<BYTE> ProtocolPacket_TMCM::to_byte_array()
{
	vector<BYTE> byteArray;
	byteArray.push_back(this->Address);
	byteArray.push_back(this->Command);
	byteArray.push_back(this->Type);
	byteArray.push_back(this->Motor);
	for (int i = 0; i < 4; i++) byteArray.push_back(this->Data[i]);
	byteArray.push_back(this->Checksum);

	return byteArray;
}
