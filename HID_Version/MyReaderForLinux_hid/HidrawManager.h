#ifndef _HIDRAW_MANAGER_H
#define _HIDRAW_MANAGER_H

#include "hidapi.h"
#include "ProtocolPacket_RFID.h"
#include "ConvertData.h"
#include "DES.h"

#define PARAMS_LEN 27

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

    bool isEntrypted;	//�Ƿ����

public:
	bool is_params_init;
	RFID_BaseParams dev_params_;	//RFID�豸����ز���

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
	int get_dev_params();
	int init_dev_params(vector<BYTE> byteArray);
	int set_dev_params();		  //Ĭ��ֵ0xff��ʾ���޸�

	/*Ӧ��ģʽ����*/
	int get_epc();								//��ȡ��ǩ
	int get_data(BYTE membank);					//��ȡ�ĸ���ͬ�洢��������
	int set_data(BYTE membank, BYTE data[]);	//���ò�ͬ�洢��������(EPC/USER)
 
 
  /*����*/
	/*userר�üӽ��ܺ���, flag = 1Ϊ���ܣ� flag = 2Ϊ����*/
	bool entrypt(BYTE(&byteArray)[8], int num);
	bool set_entrypted(bool flag);
	bool get_entrypted();
};

#endif // !HIDRAW_MANAGER_H_



