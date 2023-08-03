#ifndef _CSerialPort_H
#define _CSerialPort_H

#include "ProtocolPacket.h"

#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/

#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
#include <stdint.h>

#include <vector>
using namespace std;
class CSerialPort
{
private:
	int fd;	//linux�������豸�����ļ�����
	int Address;
	struct  termios Opt;
	int baudrate_arr[7] = { B115200, B57600, B19200, B9600 };
	int name_arr[7] = { 115200, 57600, 19200, 9600 };
	BYTE* sendbuff;

	bool isEntrypted;	//�Ƿ����

public:
	const BYTE MSG_CMD = 00;
	const BYTE MSG_GET = 50;

	const BYTE READ_EPC_ONLY = 33;
	const BYTE READ_EPC_AND_DATA = 34;
	const BYTE WRITE_DATA = 35;
	const BYTE GET_INFO = 131;		//0x83

	CSerialPort();
	~CSerialPort();

	bool Open(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//���Ӵ��ڲ����ô��ڲ���
	bool OpenPort(const char* dev);		//���Ӵ���
	bool Setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//���ô��ڲ���

	void Close();

	//����������Ͱ����ɹ����ط������ݳ��ȣ�ʧ�ܷ���0
	int Send(vector<BYTE> byteArray);
	int Send(ProtocolPacket protocolPacket);	//��װ��Э������

	//����Auto�ϴ������ݣ����ػ�ȡ�����ݳ���
	int RecvForAuto(BYTE recvByteArray[], int length);

	//Ӧ��ģʽ�Է��صİ����в��
	void DividedRTNPacket(BYTE byteArray[], int length);


	//��ö����������Ϣ
	int GetInformation();

	//Ӧ��ģʽ��ȡ��ǩ
	int IdentifyEPC();
	//Ӧ��ģʽ��ȡ�ĸ���ͬ�洢��������
	int ReadEPCAndData(BYTE membank);

	int UpdateData(BYTE membank, BYTE data[]);

	/*����*/
	//�򵥼����㷨
	bool Entrypt(BYTE* byteArray, int length);
	//BYTE* Entrypt(BYTE* byteArray, int length);

	//�򵥽����㷨���ַ�������һλ
	bool UnEntrypt(BYTE* byteArray, int length);

	bool entrypt(BYTE(&byteArray)[8], int num);

	bool SetEntrypted(bool flag);

	bool GetEntrypted();


};

#endif