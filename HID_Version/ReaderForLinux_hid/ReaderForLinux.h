#ifndef _ReaderForLinux_H
#define _ReaderForLinux_H

//#include "CSerialPort.h"
#include "HidrawManager.h"
#include "Tools.h"

#define MAX_RETRYCOUNT 6


const int GET_DEV_INFO = 1;
const int GET_SINGLE_EPC = 2;
const int GET_DATA_RFU = 3;
const int GET_DATA_EPC = 4;
const int GET_DATA_TID = 5;
const int GET_DATA_USER = 6;



int isConnected();						//判断光纤是否连接的主要函数

bool getDevInfo();						//获取读卡器固件信息

bool getSingleEPC();

bool getData(int membank);

bool get(int id);

bool setData(int membank, BYTE data[]);

bool setData_safety(int membank, BYTE data[]);

//bool entrypt(BYTE(&byteArray)[8]);			//加密user区数据
//bool unentrypt(BYTE(&byteArray)[8]);			//解密user区数据

bool entrypt(BYTE(&byteArray)[8], int num);		//加解密user区数据

bool test_before_using(int & remain_avail_time);

#endif
