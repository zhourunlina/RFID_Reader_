#include "ReaderForLinux.h"
#include "Tools.h"
#include "DES.h"
#include "ConvertData.h"
#include <iostream>
#include <cstring>
using namespace std;

extern int flag;

BYTE mac[10];							//本机mac地址
BYTE byteArray[1024];					//发送（接收）信息的字节数组
int len;								//发送（接收）信息的长度
int retryCount = MAX_RETRYCOUNT;		//失败重连的最大次数

HidrawManager manager;


int isConnected()
{
	/*Step 1*/
	/*连接HID，失败返回错误代码99*/
	if (manager.connect() < 0) return -99;
	cout << getTime() << "  " << "HID通讯成功" << endl;

	/*Step 2*/
	/*使用系统*/
	int remain_avail_time = -1;					//设置剩余使用次数

	if (test_before_using(remain_avail_time)) cout << getTime() << "  " << "使用系统成功" << endl;

	return remain_avail_time;
}



/*获取读卡器固件信息*/
bool getDevInfo(){ get(GET_DEV_INFO);}
bool getSingleEPC(){ get(GET_SINGLE_EPC); }
bool getData(int membank)
{ 
	return get(membank + 3);
}

bool get(int id)
{
	do
	{
		switch (id)
		{
		case GET_DEV_INFO: manager.get_dev_info(); break;
		case GET_SINGLE_EPC: manager.get_epc(); break;
		case GET_DATA_RFU: manager.get_data(0); break;
		case GET_DATA_EPC: manager.get_data(1); break;
		case GET_DATA_TID: manager.get_data(2); break;
		case GET_DATA_USER: manager.get_data(3); break;
		}

		len = manager.recv_for_auto_tl(byteArray, 1024, 5);
		if (len == -2)
		{
			cout << getTime() + "  等待应答超时 " << endl;
			return false;
		}
		else if (len == -1 || byteArray[4] == 1)	retryCount--;
		else  retryCount = MAX_RETRYCOUNT;

		manager.divided_rtn_packet(byteArray, len);
	} while (retryCount < MAX_RETRYCOUNT && retryCount >= 0);

	if (retryCount == -1)	return false;

	return true;
}

bool setData(int membank, BYTE data[])
{
	do
	{
		manager.set_data(membank, data);

		len = manager.recv_for_auto_tl(byteArray, 1024, 5);
		if (len == -2)
		{
			cout << getTime() + "  等待应答超时 " << endl;
			return false;
		}
		else if (len == -1 || byteArray[4] == 1)	retryCount--;
		else  retryCount = MAX_RETRYCOUNT;

		manager.divided_rtn_packet(byteArray, len);
	} while (retryCount < MAX_RETRYCOUNT && retryCount >= 0);

	if (retryCount == -1)	return false;

	return true;
}

//setData_safety函数应保证更新后的数据是与预期数据完全一致的，因此需要额外存在错误检测机制。
bool setData_safety(int membank, BYTE data[])
{
	bool is_updated_ = false;
	BYTE user[8];
	int idx = 1;

	while (!is_updated_)
	{
		//cout << idx++ << "轮更新开始" << endl;
		if (setData(3, data))
		{
			getData(3);

			/*获取USER数据*/
			for (int i = 0; i < 8; i++)
			{
				//user[i] = byteArray[i + 21];
				is_updated_ = true;
				if (data[i] != byteArray[i + 21]) is_updated_ = false;
			}
			cout << "is_updated_" << is_updated_ << endl;
		}
	}

	return true;
}


/*加解密函数, flag = 1为加密， flag = 2为解密*/
bool entrypt(BYTE(&byteArray)[8], int num)
{
	miyao();
	string temp, str[2];
	for (int i = 0; i < 8; i++) temp += hex_to_binary(byteArray[i]);
	str[0] = temp;

	flag = num;
	temp = wen(str, 1);			//解密后二进制字符串

	auto narray = bstring_to_byte_array(temp);

	for (int i = 0; i < 8; i++) byteArray[i] = narray[i];
}


bool test_before_using(int & remain_avail_time)
{
	BYTE epc[12], user[8], user_copy[8];		//12位epc标签，8位user数据
	bool isInited = false;		//isInited表示user数据被初始化，默认为false;

	if(!getData(3)) //获取user,应答字节流存储在byteArray，长度为len
  {
    remain_avail_time = -1;
    return false;	
  }

	/*获取EPC和USER数据*/
	for (int i = 0; i < 12; i++) epc[i] = byteArray[i + 9];
	for (int i = 0; i < 8; i++)  user_copy[i] = user[i] = byteArray[i + 21];
	
	/*解密 flag = 2*/
	entrypt(user, 2);

	for (int i = 0; i < 8; i++) if (user[i]) isInited = true;			//不全为0则已被初始化
 
	/*如果USER区未被初始化则初始化*/
	if (!isInited)
	{
		getMac(mac);
		for (int i = 0; i < 6; i++) user[i] = mac[i];		//前六位为本机mac地址
		user[6] = 0x00;										//第七位为0
		user[7] = epc[11];									//第八位为最大使用次数,由标签最后一位决定
	}
	else
	{
		/*匹配EPC标签*/
		if (is_epc_pattern(byte_array_to_string(epc, 0, 12))) cout << getTime() << "  " <<"标签匹配成功" << endl;
		else
    {
      remain_avail_time = -2;
      return false;
    }

		/*匹配主机*/
		if (is_user_pattern(byte_array_to_string(user, 0, 8))) cout << getTime() << "  " << "主机匹配成功" << endl;
		else
    {
      remain_avail_time = -3;
      return false;
    }
	}

	/*输出使用前EPC和USER数据*/
	printf("\n-----------------Before RFID_INFO -----------------\n");
	printf("EPC     : ");
	for (int i = 0; i < 12; i++) printf("%02X ", epc[i]);
	printf("\nUSER(E) : ");
	for (int i = 0; i < 8; i++) printf("%02X ", user_copy[i]);
	printf("\nUSER(U) : ");
	for (int i = 0; i < 8; i++) printf("%02X ", user[i]);
	printf("\n\n");
	

	/*使用系统*/
	if (user[7])
	{
		user[7] -= 1;
   
   remain_avail_time = (int)user[7];				//作为剩余使用次数返回

		for (int i = 0; i < 8; i++) user_copy[i] = user[i];

		/*加密*/
		entrypt(user, 1);

		setData_safety(3, user);

		/*输出使用前EPC和USER数据*/
		printf("\n-----------------After RFID_INFO -----------------\n");
		printf("EPC     : ");
		for (int i = 0; i < 12; i++) printf("%02X ", epc[i]);
		printf("\nUSER(E) : ");
		for (int i = 0; i < 8; i++) printf("%02X ", user[i]);
		printf("\nUSER(U) : ");
		for (int i = 0; i < 8; i++) printf("%02X ", user_copy[i]);
		printf("\n\n");
		
	}
	else
  {
    remain_avail_time = -4;
    return false;
  }

	return true;
}



int main()
{
  for (int i = 0; i < 55; i++)
  {
	  
  
  int avatime = isConnected();

  if (avatime < 0)
  {
    switch (avatime)
	  {
	  case -1:cout << getTime() << "  " << "使用系统失败:未找到标签" << endl; break;
	  case -2:cout << getTime() << "  " << "使用系统失败:标签匹配失败" << endl; break;
	  case -3:cout << getTime() << "  " << "使用系统失败:主机匹配失败" << endl; break;
      case -4:cout << getTime() << "  " << "使用系统失败:可用次数耗尽" << endl; break;
	  case -99:cout << getTime() << "  " << "使用系统失败:串口通讯失败" << endl; break;
	  }
    return -1;
  }
	
  cout << getTime() << "  " << "可用使用次数:" << " " << avatime << endl;
  }

	return 0;
}
