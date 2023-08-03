#ifndef _CSerialPort_H
#define _CSerialPort_H

#include "ProtocolPacket.h"

#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/

#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include <stdint.h>

#include <vector>
using namespace std;
class CSerialPort
{
private:
	int fd;	//linux将串口设备当做文件处理
	int Address;
	struct  termios Opt;
	int baudrate_arr[7] = { B115200, B57600, B19200, B9600 };
	int name_arr[7] = { 115200, 57600, 19200, 9600 };
	BYTE* sendbuff;

	bool isEntrypted;	//是否加密

public:
	const BYTE MSG_CMD = 00;
	const BYTE MSG_GET = 50;

	const BYTE READ_EPC_ONLY = 33;
	const BYTE READ_EPC_AND_DATA = 34;
	const BYTE WRITE_DATA = 35;
	const BYTE GET_INFO = 131;		//0x83

	CSerialPort();
	~CSerialPort();

	bool Open(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//连接串口并配置串口参数
	bool OpenPort(const char* dev);		//连接串口
	bool Setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//配置串口参数

	void Close();

	//向读卡器发送包，成功返回发送数据长度，失败返回0
	int Send(vector<BYTE> byteArray);
	int Send(ProtocolPacket protocolPacket);	//封装到协议类中

	//接受Auto上传的数据，返回获取的数据长度
	int RecvForAuto(BYTE recvByteArray[], int length);

	//应答模式对返回的包进行拆包
	void DividedRTNPacket(BYTE byteArray[], int length);


	//获得读卡器相关信息
	int GetInformation();

	//应答模式获取标签
	int IdentifyEPC();
	//应答模式获取四个不同存储区的数据
	int ReadEPCAndData(BYTE membank);

	int UpdateData(BYTE membank, BYTE data[]);

	/*加密*/
	//简单加密算法
	bool Entrypt(BYTE* byteArray, int length);
	//BYTE* Entrypt(BYTE* byteArray, int length);

	//简单解密算法，字符串逆移一位
	bool UnEntrypt(BYTE* byteArray, int length);

	bool entrypt(BYTE(&byteArray)[8], int num);

	bool SetEntrypted(bool flag);

	bool GetEntrypted();


};

#endif