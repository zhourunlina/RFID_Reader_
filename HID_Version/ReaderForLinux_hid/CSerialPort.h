#ifndef _CSerialPort_H
#define _CSerialPort_H

#include "ProtocolPacket_RFID.h"
#include "ProtocolPacket_TMCM.h"
#include "ProtocolPacket_XMT.h"

#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/

#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include <stdint.h>
#include <sys/ioctl.h>

using namespace std;


class CSerialPort
{
private:
	int fd;						//linux将串口设备当做文件处理
	int port_addr;
	struct termios options;
	int baudrate_arr[7] = { B115200, B57600, B19200, B9600 };
	int name_arr[7] = { 115200, 57600, 19200, 9600 };
	BYTE* sendbuff;

public:
	CSerialPort();
	~CSerialPort();

	/*串口连接 配置 和关闭*/
	bool open_and_setup(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity);	//连接串口并配置串口参数
	bool open_port(const char* dev);																			//连接串口
	bool setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity);							//配置串口参数

	void close_port();

	/*向读卡器发送包，成功返回发送数据长度，失败返回0*/
	int send(vector<BYTE> byteArray);
	int send(ProtocolPacket_RFID protocolPacket);															//封装到协议类中
  int send(ProtocolPacket_TMCM protocolPacket);                         //封装到TMCM协议类中
  int send(ProtocolPacket_XMT protocolPacket);													//封装到XMT协议类中

	/*接受Auto上传的数据，返回获取的数据长度*/
	int recv_for_auto(BYTE recvByteArray[], int length);
  int recv_for_auto_tl(BYTE recvByteArray[], int length, int time);	  //带有时间限制的读取,时间单位为s

	/*应答模式对返回的包进行拆包*/
	void divided_rtn_packet(BYTE byteArray[], int length);


	/*获得读卡器固件相关信息*/
	int get_dev_info();

	/*应答模式功能*/
	int get_epc();								//获取标签
	int get_data(BYTE membank);					//获取四个不同存储区的数据
	int set_data(BYTE membank, BYTE data[]);	//设置不同存储区的数据(EPC/USER)
};


#endif
