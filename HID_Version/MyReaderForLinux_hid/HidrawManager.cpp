#include "HidrawManager.h"
#include <iostream>
using namespace std;

extern int flag;

HidrawManager::HidrawManager()
{
	hid_init();
	port_addr = 65535;
	sendbuff = NULL;
	is_params_init = false;
}

HidrawManager::~HidrawManager()
{
	close();
	hid_exit();
}

int HidrawManager::connect()
{
	dev_info_ = hid_enumerate(vendor_id, product_id);
	//û�ж����豸��Ϣ
	if (!dev_info_) return -1;

	//����vid pid���豸
	dev_handle_ = hid_open(vendor_id, product_id, NULL);
	//û�гɹ����豸
	if (!dev_handle_) return -2;

	return 0;
}

void HidrawManager::close()
{
	if (dev_info_ != NULL)
	{
		hid_free_enumeration(dev_info_);
		dev_info_ = NULL;
	}
	if (dev_handle_ != NULL)
	{
		hid_close(dev_handle_);
		dev_handle_ = NULL;
	}

	return;
}

int HidrawManager::send(vector<BYTE> byteArray)
{
	int cmd_len_ = byteArray.size();
	sendbuff = new BYTE[cmd_len_ + 2];	// +2����һλΪHID��Ҫ��report id�� �ڶ�λΪ����������ĳ���
	sendbuff[0] = 0x00;					//��λ���report id
	sendbuff[1] = (BYTE)cmd_len_;		//�ڶ�λΪcmd����
	for (int i = 0; i < cmd_len_; i++)
		sendbuff[i + 2] = byteArray[i];
	int res = hid_write(dev_handle_, sendbuff, cmd_len_ + 2);
	delete[] sendbuff;

	return res;
}

int HidrawManager::send(ProtocolPacket_RFID protocolPacket)
{
	protocolPacket.show_protocol_packet();
	return send(protocolPacket.to_byte_array());
}

int HidrawManager::recv_for_auto(BYTE recvByteArray[], int length)
{
	BYTE tmp[65];	//64 + 1, 1����report id����ʵ����1Ҳ�У��ռ�´�������
	int res = hid_read(dev_handle_, tmp, 65);	
	if (res > 1)
	{
		int rsp_len_ = (int)tmp[0];
		for (int i = 0; i < rsp_len_; i++)
			recvByteArray[i] = tmp[i + 1];
		return rsp_len_;
	}
		
	return res - 1;
}

int HidrawManager::recv_for_auto_tl(BYTE recvByteArray[], int length, int time)	  //����ʱ�����ƵĶ�ȡ,ʱ�䵥λΪs
{
	BYTE tmp[65];	//64 + 1, 1����report id����ʵ����1Ҳ�У��ռ�´�������
	int res = hid_read_timeout(dev_handle_, tmp, 65, time * 1000);	  //����ɺ���
	if (res > 1)
	{
		int rsp_len_ = (int)tmp[0];
		for (int i = 0; i < rsp_len_; i++)
			recvByteArray[i] = tmp[i + 1];
		return rsp_len_;
	}

	return res - 1;
}

void HidrawManager::divided_rtn_packet(BYTE byteArray[], int length)
{
	if (length < 7)  return;

	int num;
	for (num = 0; num < length; num++)
	{
		if (byteArray[num] != 3)    continue;       //�ҵ���һ��03����Ӧ�����ʼλ��־

		/*
		num + 5 ΪLength��Length��ָINFO�ĳ��ȣ�
		Э��涨�������СΪ7�ֽڣ���������ʼλ��1��+ ��ַ��2��+ CID1��1��+ CID2��1��+ Length��1��+ INFO��>=1�� + CheckSum��1��
		*/
		if (length - num < 7 + byteArray[num + 5])  return;

		/*num + 1 Ϊ�͵�ַ�� num + 2 Ϊ�ߵ�ַ�� |Ϊλ����Ļ�����*/
		int addr = byteArray[num + 1] | (byteArray[num + 2] << 8);

		/*CID2 �ڻش��б���Ϊ������ RTN ��00Ϊ������01Ϊ����02λ����ؿ��ţ�05Ϊ�������Ϳ���*/
		BYTE rtn = (BYTE)(byteArray[num + 4] & 0x7f);
		if (rtn == 0 || rtn == 1 || rtn == 2 || rtn == 5)
		{
			if (port_addr != addr) port_addr = addr;
			break;
		}
	}


	length -= num;                                //ɾ����ʼλ��־ǰ���ַ���Ϣ,��num��Ϊ��Ч��Ϣ�ĵ�һλ
	vector<BYTE> cmdByteArray;

	while (length >= 7)
	{
		int cmdLen = 7 + byteArray[num + 5];      ////byteArray[num + 5]��infoLen��������Info��DATA���ĳ���;
		if (length >= cmdLen)
		{
			for (int i = 0; i < cmdLen; i++)
				cmdByteArray.push_back(byteArray[num + i]);

			num += cmdLen, length -= cmdLen;

			ProtocolPacket_RFID* pst = new ProtocolPacket_RFID(cmdByteArray);
			pst->show_protocol_packet();

			cmdByteArray.clear();
		}
	}
}

int HidrawManager::get_dev_info()
{
	return send(ProtocolPacket_RFID(this->port_addr, GET_INFO, MSG_GET));
}

int HidrawManager::get_dev_params()
{
	return send(ProtocolPacket_RFID(this->port_addr, DEV_PARAMS, MSG_GET));
}

int HidrawManager::set_dev_params()
{
	vector<BYTE> param;
	param.push_back(this->dev_params_.output);

	param.push_back(this->dev_params_.workmode);

	param.push_back(this->dev_params_.readtype);
	param.push_back(this->dev_params_.readinterval);
	param.push_back(this->dev_params_.readdelay);
	for (int i = 0; i < 4; i++) param.push_back(this->dev_params_.wg[i]);
	for (int i = 0; i < 2; i++) param.push_back(this->dev_params_.sameid[i]);
	param.push_back(this->dev_params_.buzzer);
	for (int i = 0; i < 4; i++) param.push_back(this->dev_params_.maccess[i]);
	param.push_back(this->dev_params_.mmem);
	param.push_back(this->dev_params_.mstart);
	param.push_back(this->dev_params_.mlength);
	param.push_back(this->dev_params_.mstart1);
	param.push_back(this->dev_params_.mlength1);
	param.push_back(this->dev_params_.mstart2);
	param.push_back(this->dev_params_.mlength2);
	for (int i = 0; i < 4; i++) param.push_back(this->dev_params_.mkey[i]);

	return send(ProtocolPacket_RFID(this->port_addr, DEV_PARAMS, MSG_SET, param));
}

int HidrawManager::init_dev_params(vector<BYTE> byteArray)
{
	if (byteArray.size() != PARAMS_LEN) return -1;

	this->dev_params_.output = byteArray[0];
	this->dev_params_.workmode = byteArray[1];
	this->dev_params_.readtype = byteArray[2];
	this->dev_params_.readinterval = byteArray[3];
	this->dev_params_.readdelay = byteArray[4];
	for (int i = 0; i < 4; i++) this->dev_params_.wg[i] = byteArray[i + 5];
	for (int i = 0; i < 2; i++) this->dev_params_.sameid[i] = byteArray[i + 9];
	this->dev_params_.buzzer = byteArray[11];
	for (int i = 0; i < 4; i++) this->dev_params_.maccess[i] = byteArray[i + 12];
	this->dev_params_.mmem = byteArray[16];
	this->dev_params_.mstart = byteArray[17];
	this->dev_params_.mlength = byteArray[18];
	this->dev_params_.mstart1 = byteArray[19];
	this->dev_params_.mlength1 = byteArray[20];
	this->dev_params_.mstart2 = byteArray[21];
	this->dev_params_.mlength2 = byteArray[22];
	for (int i = 0; i < 4; i++) this->dev_params_.mkey[i] = byteArray[i + 23];

	is_params_init = true;

	return 0;
}

int HidrawManager::get_epc()
{
	return send(ProtocolPacket_RFID(this->port_addr, READ_EPC_ONLY, MSG_CMD));
}

int HidrawManager::get_data(BYTE membank)
{
	vector<BYTE> param;

	for (int i = 0; i < 4; i++) param.push_back(0x00);  //accPSW ȷ�����룬ȱʡֵΪ 00 00 00 00��

	param.push_back(membank);                           //�洢����

	BYTE startAddr, dataLen;
	switch (membank)
	{
	case 0: startAddr = 0x00; dataLen = 0x04; break;    //RFU
	case 1: startAddr = 0x02; dataLen = 0x06; break;    //EPC
	case 2: startAddr = 0x00; dataLen = 0x06; break;    //TID
	case 3: startAddr = 0x00; dataLen = 0x04; break;    //USER
	}
	param.push_back(startAddr);                         //��ʼ��ַ
	param.push_back(dataLen);                           //��ȡ���ݳ���

	return send(ProtocolPacket_RFID(this->port_addr, READ_EPC_AND_DATA, MSG_CMD, param));
}

int HidrawManager::set_data(BYTE membank, BYTE data[])
{
	vector<BYTE> param;

	for (int i = 0; i < 4; i++) param.push_back(0x00);  //accPSW ȷ�����룬ȱʡֵΪ 00 00 00 00��

	param.push_back(membank);                           //�洢����

	BYTE startAddr, dataLen;
	switch (membank)
	{
	case 1: startAddr = 0x02; dataLen = 0x06; break;    //EPC
	case 3: startAddr = 0x00; dataLen = 0x04; break;    //USER
	}
	param.push_back(startAddr);                         //��ʼ��ַ
	param.push_back(dataLen);                           //��ȡ���ݳ���

	for (int i = 0; i < dataLen * 2; i++) param.push_back(data[i]);

	return send(ProtocolPacket_RFID(this->port_addr, WRITE_DATA, MSG_CMD, param));
}

/*userר�üӽ��ܺ���, flag = 1Ϊ���ܣ� flag = 2Ϊ����*/
bool HidrawManager::entrypt(BYTE(&byteArray)[8], int num)
{
    miyao();
    string temp, str[2];
    for (int i = 0; i < 8; i++) temp += hex_to_binary(byteArray[i]);
    str[0] = temp;

    flag = num;
    temp = wen(str, 1);			//���ܺ�������ַ���

    auto narray = BinaryStringToByteArray(temp);

    for (int i = 0; i < 8; i++) byteArray[i] = narray[i];
}

bool HidrawManager::set_entrypted(bool flag)
{
    this->isEntrypted = flag;
    return isEntrypted == flag;
}

bool HidrawManager::get_entrypted()
{
    return isEntrypted;
}