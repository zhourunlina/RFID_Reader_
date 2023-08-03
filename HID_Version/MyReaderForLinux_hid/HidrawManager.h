#ifndef _HIDRAW_MANAGER_H
#define _HIDRAW_MANAGER_H

#include "hidapi.h"
#include "ProtocolPacket_RFID.h"
#include "ConvertData.h"
#include "DES.h"

#define PARAMS_LEN 27

//设备的pid和vid，根据要读的设备设置
const unsigned short vendor_id = 0x04d8;
const unsigned short product_id = 0x033f;

class HidrawManager
{
private:
	int port_addr;				//与RFID设备根据协议通信所需
	hid_device* dev_handle_;
	hid_device_info* dev_info_;
	BYTE* sendbuff;

    bool isEntrypted;	//是否加密

public:
	bool is_params_init;
	RFID_BaseParams dev_params_;	//RFID设备的相关参数

	HidrawManager();
	~HidrawManager();

	/*HID连接 和关闭*/
	int connect();
	void close();

	/*向读卡器发送包*/
	int send(vector<BYTE> byteArray);					
	int send(ProtocolPacket_RFID protocolPacket);		//封装到协议类中

	/*接收上传的数据，返回获取的数据长度*/
	int recv_for_auto(BYTE recvByteArray[], int length);
    int recv_for_auto_tl(BYTE recvByteArray[], int length, int time);	  //带有时间限制的读取,时间单位为s

	/*应答模式对返回的包进行拆包*/
	void divided_rtn_packet(BYTE byteArray[], int length);

	/*获得读卡器固件相关信息*/
	int get_dev_info();
	int get_dev_params();
	int init_dev_params(vector<BYTE> byteArray);
	int set_dev_params();		  //默认值0xff表示不修改

	/*应答模式功能*/
	int get_epc();								//获取标签
	int get_data(BYTE membank);					//获取四个不同存储区的数据
	int set_data(BYTE membank, BYTE data[]);	//设置不同存储区的数据(EPC/USER)
 
 
  /*加密*/
	/*user专用加解密函数, flag = 1为加密， flag = 2为解密*/
	bool entrypt(BYTE(&byteArray)[8], int num);
	bool set_entrypted(bool flag);
	bool get_entrypted();
};

#endif // !HIDRAW_MANAGER_H_



