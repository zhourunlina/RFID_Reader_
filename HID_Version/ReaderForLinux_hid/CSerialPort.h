#ifndef _CSerialPort_H
#define _CSerialPort_H

#include "ProtocolPacket_RFID.h"
#include "ProtocolPacket_TMCM.h"
#include "ProtocolPacket_XMT.h"

#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/

#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
#include <stdint.h>
#include <sys/ioctl.h>

using namespace std;


class CSerialPort
{
private:
	int fd;						//linux�������豸�����ļ�����
	int port_addr;
	struct termios options;
	int baudrate_arr[7] = { B115200, B57600, B19200, B9600 };
	int name_arr[7] = { 115200, 57600, 19200, 9600 };
	BYTE* sendbuff;

public:
	CSerialPort();
	~CSerialPort();

	/*�������� ���� �͹ر�*/
	bool open_and_setup(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//���Ӵ��ڲ����ô��ڲ���
	bool open_port(const char* dev);																			//���Ӵ���
	bool setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity);							//���ô��ڲ���

	void close_port();

	/*����������Ͱ����ɹ����ط������ݳ��ȣ�ʧ�ܷ���0*/
	int send(vector<BYTE> byteArray);
	int send(ProtocolPacket_RFID protocolPacket);															//��װ��Э������
  int send(ProtocolPacket_TMCM protocolPacket);                         //��װ��TMCMЭ������
  int send(ProtocolPacket_XMT protocolPacket);													//��װ��XMTЭ������

	/*����Auto�ϴ������ݣ����ػ�ȡ�����ݳ���*/
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


#endif
