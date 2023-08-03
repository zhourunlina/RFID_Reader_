#ifndef _ProtocolPacket_XMT_H
#define _ProtocolPacket_XMT_H

#include <vector>
using namespace std;

typedef unsigned char BYTE;



class ProtocolPacket_XMT
{
public:
	BYTE Preamble = 0xAA;		//��ʼ�ֽ�
	BYTE Address = 0;		//��ַ 0Ϊ�㲥(����ַΪ0ʱ,ȫ����λ������,�趨��ַʱ���ù㲥) 1��255Ϊ��λ����ַ
	BYTE Length = 0;		//����
	BYTE B3 = 0;		//ָ����1
	BYTE B4 = 0x02;		//ָ����2 0x02��������
	BYTE Data[256];		//���ݶ�
	BYTE Checksum = 0;		//У���


	ProtocolPacket_XMT();
	ProtocolPacket_XMT(vector<BYTE> ArgByte);
	ProtocolPacket_XMT(BYTE nB3, vector<BYTE> ArgByte);


	vector<BYTE> build_protocol_packet(int nAddr, BYTE nLength, BYTE nB3, BYTE nB4, vector<BYTE> ArgByte);

	BYTE checksum();

	void show_protocol_packet();

	vector<BYTE> to_byte_array();
};
#endif

