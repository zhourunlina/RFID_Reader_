#include "CSerialPort.h"
#include "DES.h"
#include "ConvertData.h"
#include <string.h>
#include <sys/ioctl.h>
#include <iostream>
#include <vector>
using namespace std;

extern int flag;

CSerialPort::CSerialPort()
{
    fd = -1;
    Address = 65535;
    sendbuff = NULL;
}
CSerialPort::~CSerialPort() 
{

}

bool CSerialPort::Open(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity)
{
    if (!OpenPort(dev))
        return false;
    if (!Setup(57600, 0, 8, 1, 'N'))
        return false;
    return true;
}

bool CSerialPort::OpenPort(const char* dev)
{

    char* _dev = new char[32];
    strcpy(_dev, dev);
    fd = open(_dev, O_RDWR);         //O_RDWR ֧�ֶ�д| O_NOCTTY | O_NDELAY   
    if (-1 == fd)
        return false;

    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(fd, TIOCMBIS, &DTR_flag);//Set RTS pin
    return true;

}

bool CSerialPort::Setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity)
{
    int   status;
    struct termios options;
    /*tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú��������Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1.
    */
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1, ���ڵ���ʧ��");
        return false;
    }

    //���ô������벨���ʺ����������  
    for (int i = 0; i < sizeof(baudrate_arr) / sizeof(int); i++)
    {
        if (baudrate == name_arr[i])
        {
            cfsetispeed(&options, baudrate_arr[i]);
            cfsetospeed(&options, baudrate_arr[i]);
        }
    }

    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;

    switch (flow_ctrl)  //����������  
    {
    case 0://��ʹ��������  
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1://ʹ��Ӳ��������  
        options.c_cflag |= CRTSCTS;
        break;
    case 2://ʹ�����������  
        options.c_cflag |= IXON | IXOFF | IXANY;
        break;
    }

    //����������־λ  
    options.c_cflag &= ~CSIZE;   
    switch (databits)       //����λ  
    {
    case 5:
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return false;
    }

    switch (stopbits)   //ֹͣλ   
    {
    case 1:
        options.c_cflag &= ~CSTOPB; break;
    case 2:
        options.c_cflag |= CSTOPB; break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return false;
    }

    switch (parity) //У��λ  
    {
    case 'n':
    case 'N':       //����żУ��λ��  
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':       //����Ϊ��У��      
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':       //����ΪżУ��    
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S':       //����Ϊ�ո�   
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return false;
    }
 


    //�޸����ģʽ��ԭʼ�������  
    options.c_oflag &= ~OPOST;
    
    //�޸�����ģʽ, ��������ַ���ʧ����, ��0Dǿ�Ʊ�0A�� ���ղ���0x11, 0x0d,0x13
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);  

    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 10; /* ��ȡһ���ַ��ȴ�10*(1/10)s */
    options.c_cc[VMIN] = 50; /* ��ȡ�ַ������ٸ���Ϊ50 */
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd, TCIFLUSH);

    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("com set error!\n");
        return false;
    }
    return true;
}

void CSerialPort::Close()
{
    close(fd);
}


int CSerialPort::Send(vector<BYTE> byteArray)
{
   sendbuff = new BYTE[byteArray.size()];
   for (int i = 0; i < byteArray.size(); i++)
       sendbuff[i] = byteArray[i];
    return write(fd, sendbuff, byteArray.size());
}
int CSerialPort::Send(ProtocolPacket protocolPacket)
{
    protocolPacket.showProtocolPacket();
    return Send(protocolPacket.ToVectorArray());
}

int CSerialPort::RecvForAuto(BYTE recvByteArray[], int length)
{
    return read(fd, recvByteArray, length);    //read�������ص��ַ�����ʵ�ʴ����յ����ַ���
}


int CSerialPort::GetInformation()
{
    return Send(ProtocolPacket(65535, GET_INFO, MSG_GET));
}

//Ӧ��ģʽ�Է��صİ����в��
void CSerialPort::DividedRTNPacket(BYTE byteArray[], int length)
{
    if (length == 0 || length < 7)  return;

    int num = 0;
    for (num; num < length; num++)
    {
        //�ҵ���һ��03������ʼλ��־
        if (byteArray[num] != 3)    continue;

        //num + 5 ΪLength��Length��ָINFO�ĳ��ȣ�Э��涨�������СΪ7�ֽڣ���������ʼλ��1��+ ��ַ��2��+ CID1��1��+ CID2��1��+ Length��1��+ INFO��>=1�� + CheckSum��1��
        if (length - num < 7 || length - num < 7 + byteArray[num + 5])  return;

        //num + 1 Ϊ�͵�ַ�� num + 2 Ϊ�ߵ�ַ�� |Ϊλ����Ļ�����
        int addr = byteArray[num + 1] | (byteArray[num + 2] << 8);

        //CID 2 �ڻش��б���Ϊ������ RTN ��00Ϊ������01Ϊ����02λ����ؿ��ţ�05Ϊ�������Ϳ���
        BYTE b = (BYTE)(byteArray[num + 4] & 0x7f);
        if (b == 0 || b == 1 || b == 2 || b == 5)
        {
            if (Address != addr)    Address = addr;
            break;
        }
    }

    //ɾ����ʼλ��־ǰ���ַ���Ϣ,��num��Ϊ��Ч��Ϣ�ĵ�һλ
    length -= num;
    vector<BYTE> cmdByteArray;
    while (length >= 7)
    {
        //infoLen������Info��DATA���ĳ���
        int infoLen = byteArray[num + 5];
        if (length >= infoLen + 7)
        {
            for (int i = 0; i < infoLen + 7; i++)
                cmdByteArray.push_back(byteArray[num + i]);
            num = num + infoLen + 7;
            length = length - infoLen - 7;
            ProtocolPacket* pst = new ProtocolPacket(cmdByteArray);
            cmdByteArray.clear();
            pst->showProtocolPacket();
            continue;
        }

        int addr_next = byteArray[num + 1] | (byteArray[num + 2] << 8);
        if (addr_next != 65535 && num != Address)   
            return;
    }
}

int CSerialPort::IdentifyEPC()
{
    return Send(ProtocolPacket(this->Address, READ_EPC_ONLY, MSG_CMD));

}

int CSerialPort::ReadEPCAndData(BYTE membank)
{
    vector<BYTE> param;
    //accPSW ȷ�����룬ȱʡֵΪ 00 00 00 00��
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    //�洢����
    param.push_back(membank);

    BYTE startAddr = 0x02;
    BYTE dataLen = 0x06;
    switch (membank)
    {
    case 0: //RFUIsChecked
        startAddr = 0x00;
        dataLen = 0x04;
        break;
    case 1: //EPCIsChecked
        startAddr = 0x02;
        dataLen = 0x06;
        break;
    case 2: //TIDIsCheaked
        startAddr = 0x00;
        dataLen = 0x06;
        break;
    case 3: //USERIsChecked
        startAddr = 0x00;
        dataLen = 0x04;
        break;
    }
    //��ʼ��ַ
    param.push_back(startAddr);
    //��ȡ���ݳ���
    param.push_back(dataLen);

    return Send(ProtocolPacket(this->Address, READ_EPC_AND_DATA, MSG_CMD, param));
}

int CSerialPort::UpdateData(BYTE membank, BYTE* data)
{
    vector<BYTE> param;
    //accPSW ȷ�����룬ȱʡֵΪ 00 00 00 00��
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    //�洢����
    param.push_back(membank);

    BYTE startAddr = 0x02;
    BYTE dataLen = 0x06;
    switch (membank)
    {
    case 1: //EPCIsChecked
        break;
    case 3: //USERIsChecked
        startAddr = 0x00;
        dataLen = 0x04;
        break;
    }
    //��ʼ��ַ
    param.push_back(startAddr);
    //��ȡ���ݳ���
    param.push_back(dataLen);
    for (int i = 0; i < dataLen * 2; i++)
        param.push_back(data[i]);

    return Send(ProtocolPacket(this->Address, WRITE_DATA, MSG_CMD, param));
}

//�򵥼����㷨���ַ���˳��һλ
bool CSerialPort::Entrypt(BYTE* byteArray, int length)
{
    BYTE temp = byteArray[length - 1];
    for (int i = length - 1; i > 0; i--)
        byteArray[i] = byteArray[i - 1];
    byteArray[0] = temp;
    return true;
}

/*userר�ü����㷨��DES�㷨*/
//BYTE* CSerialPort::Entrypt(BYTE* byteArray, int length)
//{
//    string str[2], temp = "", res;
//    for (int i = 0; i < length; i++)  temp += hex_to_binary(byteArray[i]);
//    str[0] = temp;
//    cout << "temp :" << temp << endl;
//
//    res = wen(str, 1);
//    byteArray = BinaryStringToByteArray(res);
//    for (int i = 0; i < 8; i++)
//        printf("%02X", byteArray[i]);
//    printf("\n");
//    return BinaryStringToByteArray(res);
//}


//�򵥽����㷨���ַ�������һλ
bool CSerialPort::UnEntrypt(BYTE* byteArray, int length)
{
    BYTE temp = byteArray[0];
    for (int i = 0; i < length - 1; i++)
        byteArray[i] = byteArray[i + 1];
    byteArray[length - 1] = temp;
    return true;
}

/*userר�üӽ��ܺ���, flag = 1Ϊ���ܣ� flag = 2Ϊ����*/
bool CSerialPort::entrypt(BYTE(&byteArray)[8], int num)
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

bool CSerialPort::SetEntrypted(bool flag)
{
    this->isEntrypted = flag;
    return isEntrypted == flag;
}


bool CSerialPort::GetEntrypted()
{
    return isEntrypted;
}






