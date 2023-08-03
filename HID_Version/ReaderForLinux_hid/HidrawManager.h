#ifndef _HIDRAW_MANAGER_H
#define _HIDRAW_MANAGER_H

#include "hidapi.h"
#include "ProtocolPacket_RFID.h"
#include "ConvertData.h"

//�豸��pid��vid������Ҫ�����豸����
const unsigned short vendor_id = 0x04d8;
const unsigned short product_id = 0x033f;

class HidrawManager
{
private:
	int port_addr;				//��RFID�豸����Э��ͨ������
	hid_device* dev_handle_;
	hid_device_info* dev_info_;
	BYTE* sendbuff;

public:
	HidrawManager();
	~HidrawManager();

	/*HID���� �͹ر�*/
	int connect();
	void close();

	/*����������Ͱ�*/
	int send(vector<BYTE> byteArray);					
	int send(ProtocolPacket_RFID protocolPacket);		//��װ��Э������

	/*�����ϴ������ݣ����ػ�ȡ�����ݳ���*/
	int recv_for_auto(BYTE recvByteArray[], int length);
    int recv_for_auto_tl(BYTE recvByteArray[], int length, int time);	  //����ʱ�����ƵĶ�ȡ,ʱ�䵥λΪs

	/*Ӧ��ģʽ�Է��صİ����в��*/
	void divided_rtn_packet(BYTE byteArray[], int length);

	/*��ö������̼������Ϣ*/
	int get_dev_info();

	/*Ӧ��ģʽ����*/
	int get_epc();								//��ȡ��ǩ
	int get_data(BYTE membank);					//��ȡ�ĸ���ͬ�洢��������
	int set_data(BYTE membank, BYTE data[]);	//���ò�ͬ�洢��������(EPC/USER)
};

#endif // !HIDRAW_MANAGER_H_



