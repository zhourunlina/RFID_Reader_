#include "CSerialPort.h"
#include "ConvertData.h"
#include "Tools.h"
#include "DES.h"
#include <iostream>
#include <cstring>


BYTE byteArray[1024], mac[10];		//byteArray接受返回的字节流， mac记录本机MAC地址
int len, retryCount = 2;					//len为接受返回的字节流长度, retryCount为发送失败后的自动重连次数
const char* dev = "/dev/ttyUSB1";

CSerialPort mySerialPort;

bool getInfo(int id)
{
	do
	{
		switch (id)
		{
		case 0: mySerialPort.GetInformation(); break;
		case 1: mySerialPort.ReadEPCAndData(0); break;
		case 2: mySerialPort.ReadEPCAndData(1); break;
		case 3: mySerialPort.ReadEPCAndData(2); break;
		case 4: mySerialPort.ReadEPCAndData(3); break;
		
		}
		
		len = mySerialPort.RecvForAuto(byteArray, 1024);
		if (len == -1 || byteArray[4] == 1)	retryCount--;					//len == -1 则函数执行失败， byteArray[4] == 1 则返回码RTN 为 1
		else  retryCount = 2;												//成功读取则重置自动重连次数

		cout << GetTime() + " 写入字节数：" + to_string(len) << endl ;
   for (int i = 0; i < len; i++) printf("%02X", byteArray[i]);
		printf("\n");
		mySerialPort.DividedRTNPacket(byteArray, len);
	} while (retryCount < 2 && retryCount >= 0);

	if (retryCount == -1)
	{
		retryCount = 2;
		return false;
	}
	return true;
}

bool getData(int membank)
{
	return getInfo(membank);
}

bool setData(int membank, BYTE data[])
{
	do
	{
		mySerialPort.UpdateData(membank - 1, data);

		len = mySerialPort.RecvForAuto(byteArray, 1024);
		if (len == -1 || byteArray[4] == 1)	retryCount--;					
		else  retryCount = 2;												

		cout << GetTime() + "  写入字节数：" + to_string(len) << endl;
		mySerialPort.DividedRTNPacket(byteArray, len);
	} while (retryCount < 2 && retryCount >= 0);

	if (retryCount == -1)
	{
		retryCount = 2;
		return false;
	}

	return true;
}




int main()
{
	GetMac(mac);

	
	/*Step 1*/
	/*连接串口并设置参数*/
	cout << GetTime() << "  " << "正在尝试与读写器通信..." << endl;

	if (!mySerialPort.Open(dev, 57600, 0, 8, 1, 'N'))	//连接串口并设置参数
	{
		cout << GetTime() << "  " << "串口通讯失败，请检查串口是否已连接或是断开" << endl;
		return 0;
	}
	cout << GetTime() << "  " << "串口通讯成功" << endl;



	//应答模式输入
	int inputNum;
	string dataString;
	while (1)
	{
		cout << "串口空闲..请输入（0for设备信息，1forRFU, 2forEPC, 3forTID，4forUser，5for更新EPC，6for更新User, 7for使用系统";
		if (mySerialPort.GetEntrypted())  cout << ", 8for取消加密, 9for测试加密解密）:";
		else  cout << ", 8for加密，9for测试加密解密）:";

		cin >> inputNum;

		if (inputNum == 9)
		{
			cout << "请输入需要加密的字符流:";
			cin >> dataString;
			BYTE* byteArray = HexStringToByteArray(dataString);
			BYTE updateByteArray[8];

			for (int i = 0; i < 8; i++) updateByteArray[i] = byteArray[i];

			mySerialPort.entrypt(updateByteArray, 1);
			printf("加密：");
			for (int i = 0; i < 8; i++) printf("%02X", updateByteArray[i]);
			printf("\n");
			printf("解密：");
			mySerialPort.entrypt(updateByteArray, 2);
			for (int i = 0; i < 8; i++) printf("%02X", updateByteArray[i]);
			printf("\n");

			continue;
		}

		if (inputNum == 8)
		{
			mySerialPort.SetEntrypted(!mySerialPort.GetEntrypted());	//若输入为8，则对isEntrypted取反
			cout << endl;
			continue;
		}

		if (inputNum == 7)
		{
			BYTE tag[12], data[8];				//12位EPC标签, 8位USER数据
			bool IsInited = false;				//IsInited为true表示user数据被初始化，默认为false;

			/*获取user,应答字节流存储在byteArray，长度为len*/
			if (!getData(4))
			{
				cout << GetTime() << "  " << "使用系统失败：获取标签和USER失败" << endl;
				continue;
			}

			/*获取EPC和USER数据*/
			for (int i = 0; i < 12; i++)	tag[i] = byteArray[i + 9];
			for (int i = 0; i < 8; i++)		data[i] = byteArray[i + 21];

			/*如果加密则解密*/
			if (mySerialPort.GetEntrypted())	mySerialPort.entrypt(data, 2);			//DES加解密只对user进行
      for (int i =0; i < 8; i++) printf("%02X", data[i]);
      printf("\n");
      

			for (int i = 0; i < 8; i++)		if (data[i]) IsInited = true;	//不全为0则已被初始化

			
			/*如果USER区未被初始化则初始化*/
			if (!IsInited)
			{
				for (int i = 0; i < 6; i++)		//前六位为本机MAC地址
					data[i] = mac[i];
				data[6] = 0x00;				//第七位为0
				data[7] = tag[11];			//第八位为最大使用次数,由标签最后一位决定
			}
			else
			{
				/*匹配EPC标签*/
				if (isEPCPattern(ByteArrayToString(tag, 0, 12)))	 cout << GetTime() << "  " << "标签匹配成功" << endl;
				else
				{
					cout << GetTime() << "  " << "使用系统失败：标签匹配失败" << endl;
					continue;
				}

				/*匹配主机*/
				if (isDataPattern(ByteArrayToString(data, 0, 8))) cout << GetTime() << "  " << "主机匹配成功" << endl;
				else
				{
					cout << GetTime() << "  " << "使用系统失败：主机匹配失败" << endl;
					continue;
				}
			}

			if (data[7])
			{
				data[7] -= (IsInited ? 1 : 0);

				for (int i = 0; i < 8; i++) printf("%02X",data[i]);
				printf("\n");
				if (mySerialPort.GetEntrypted())  mySerialPort.entrypt(data, 1);	//如果加密则加密
				for (int i = 0; i < 8; i++) printf("%02X", data[i]);
				printf("\n");

				if (!setData(4, data))
				{
					cout << GetTime() << "  " << "使用系统失败：更新Data失败：发送包未成功" << endl;
					continue;
				}

				if (!IsInited)	cout << GetTime() << "  " << "用户数据已初始化" << endl;
				else cout << GetTime() << "  " << "使用系统成功！" << endl;
			}
			else  cout << "使用系统失败：使用次数用完" << endl;

			retryCount = 2;

			cout << endl;
			continue;
		}

		if (inputNum == 0)
		{
			/*获取读卡器信息*/
			if (getInfo(0))	cout << GetTime() << "  " << "获取设备信息成功" << endl;
			else cout << GetTime() << "  " << "获取设备信息失败，通讯关闭" << endl;

			cout << endl;
			continue;
		}

		if (inputNum == 1 || inputNum == 2 || inputNum == 3 || inputNum == 4)
		{
			/*获取Data四个区的数据*/
			if (getData(inputNum))
			{
				switch (inputNum)
				{
				case 1: cout << GetTime() << "  " << "获取标签和RFU成功" << endl; break;
				case 2: cout << GetTime() << "  " << "获取标签成功" << endl; break;
				case 3: cout << GetTime() << "  " << "获取标签和TID成功" << endl; break;
				case 4: cout << GetTime() << "  " << "获取标签和USER成功" << endl; break;
				}
			}
			else
			{
				switch (inputNum)
				{
				case 1: cout << GetTime() << "  " << "获取标签和RFU失败" << endl; break;
				case 2: cout << GetTime() << "  " << "获取标签失败" << endl; break;
				case 3: cout << GetTime() << "  " << "获取标签和TID失败" << endl; break;
				case 4: cout << GetTime() << "  " << "获取标签和USER失败" << endl; break;
				}
			}

			cout << endl;
			continue;
		}

		if (inputNum == 5)
		{
			cout << "请输入新的标签值(或仅输入默认最大使用次数)：";

			cin >> dataString;

			if (dataString.length() != 2 && dataString.length() != 24)
			{
				cout << "更新Data失败：输入标签长度错误" << endl;
				cout << endl;
				continue;
			}
			if (dataString.length() == 2)	dataString = GetFullTime() + "00000000" + dataString;	//仅输入标签值的最大使用次数

			BYTE* updateByteArray = HexStringToByteArray(dataString);
			if (updateByteArray == NULL)
			{
				cout << "更新Data失败：输入格式不能识别为十六进制" << endl;
				cout << endl;
				continue;
			}
      
      if (setData(2, updateByteArray)) cout << GetTime() << "  " << "更新标签成功" << endl;
			else cout << GetTime() << "  " << "更新标签失败" << endl;

			cout << endl;
			continue;
					
		}

		if (inputNum == 6)
		{
			cout << "请输入新的用户数据值：";

			cin >> dataString;
			if (dataString.length() != 16)
			{
				cout << "更新Data失败：输入USER长度错误" << endl;
				cout << endl;
				continue;
			}

			BYTE* byteArray = HexStringToByteArray(dataString);
			if (byteArray == NULL)
			{
				cout << "更新Data失败：输入格式不能识别为十六进制" << endl;
				cout << endl;
				continue;
			}
			BYTE updateByteArray[8];

			for (int i = 0; i < 8; i++) updateByteArray[i] = byteArray[i];

			if (mySerialPort.GetEntrypted())  mySerialPort.entrypt(updateByteArray, 1);	//如果加密则加密

			if (setData(4, updateByteArray)) cout << GetTime() << "  " << "更新USER区Data成功" << endl;
			else cout << GetTime() << "  " << "更新USER区Data失败" << endl;

			cout << endl;
			continue;
		}

	}

	system("pause");
	return 0;
}