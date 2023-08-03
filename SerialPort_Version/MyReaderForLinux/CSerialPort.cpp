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
    fd = open(_dev, O_RDWR);         //O_RDWR 支持读写| O_NOCTTY | O_NDELAY   
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
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1, 串口调用失败");
        return false;
    }

    //设置串口输入波特率和输出波特率  
    for (int i = 0; i < sizeof(baudrate_arr) / sizeof(int); i++)
    {
        if (baudrate == name_arr[i])
        {
            cfsetispeed(&options, baudrate_arr[i]);
            cfsetospeed(&options, baudrate_arr[i]);
        }
    }

    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;

    switch (flow_ctrl)  //数据流控制  
    {
    case 0://不使用流控制  
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1://使用硬件流控制  
        options.c_cflag |= CRTSCTS;
        break;
    case 2://使用软件流控制  
        options.c_cflag |= IXON | IXOFF | IXANY;
        break;
    }

    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;   
    switch (databits)       //数据位  
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

    switch (stopbits)   //停止位   
    {
    case 1:
        options.c_cflag &= ~CSTOPB; break;
    case 2:
        options.c_cflag |= CSTOPB; break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return false;
    }

    switch (parity) //校验位  
    {
    case 'n':
    case 'N':       //无奇偶校验位。  
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':       //设置为奇校验      
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':       //设置为偶校验    
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S':       //设置为空格   
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return false;
    }
 


    //修改输出模式，原始数据输出  
    options.c_oflag &= ~OPOST;
    
    //修改输入模式, 解决特殊字符丢失问题, 如0D强制变0A， 接收不到0x11, 0x0d,0x13
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);  

    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 10; /* 读取一个字符等待10*(1/10)s */
    options.c_cc[VMIN] = 50; /* 读取字符的最少个数为50 */
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd, TCIFLUSH);

    //激活配置 (将修改后的termios数据设置到串口中）  
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
    return read(fd, recvByteArray, length);    //read函数返回的字符数是实际串口收到的字符数
}


int CSerialPort::GetInformation()
{
    return Send(ProtocolPacket(65535, GET_INFO, MSG_GET));
}

//应答模式对返回的包进行拆包
void CSerialPort::DividedRTNPacket(BYTE byteArray[], int length)
{
    if (length == 0 || length < 7)  return;

    int num = 0;
    for (num; num < length; num++)
    {
        //找到第一个03，即起始位标志
        if (byteArray[num] != 3)    continue;

        //num + 5 为Length，Length特指INFO的长度；协议规定命令包最小为7字节，包括：起始位（1）+ 地址（2）+ CID1（1）+ CID2（1）+ Length（1）+ INFO（>=1） + CheckSum（1）
        if (length - num < 7 || length - num < 7 + byteArray[num + 5])  return;

        //num + 1 为低地址， num + 2 为高地址， |为位运算的或运算
        int addr = byteArray[num + 1] | (byteArray[num + 2] << 8);

        //CID 2 在回答中表现为返回码 RTN ，00为正常，01为错误，02位命令返回卡号，05为主动上送卡号
        BYTE b = (BYTE)(byteArray[num + 4] & 0x7f);
        if (b == 0 || b == 1 || b == 2 || b == 5)
        {
            if (Address != addr)    Address = addr;
            break;
        }
    }

    //删除起始位标志前的字符消息,以num作为有效信息的第一位
    length -= num;
    vector<BYTE> cmdByteArray;
    while (length >= 7)
    {
        //infoLen即返回Info（DATA）的长度
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
    //accPSW 确认密码，缺省值为 00 00 00 00；
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    //存储区域
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
    //起始地址
    param.push_back(startAddr);
    //读取数据长度
    param.push_back(dataLen);

    return Send(ProtocolPacket(this->Address, READ_EPC_AND_DATA, MSG_CMD, param));
}

int CSerialPort::UpdateData(BYTE membank, BYTE* data)
{
    vector<BYTE> param;
    //accPSW 确认密码，缺省值为 00 00 00 00；
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    param.push_back(0x00);
    //存储区域
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
    //起始地址
    param.push_back(startAddr);
    //读取数据长度
    param.push_back(dataLen);
    for (int i = 0; i < dataLen * 2; i++)
        param.push_back(data[i]);

    return Send(ProtocolPacket(this->Address, WRITE_DATA, MSG_CMD, param));
}

//简单加密算法，字符串顺移一位
bool CSerialPort::Entrypt(BYTE* byteArray, int length)
{
    BYTE temp = byteArray[length - 1];
    for (int i = length - 1; i > 0; i--)
        byteArray[i] = byteArray[i - 1];
    byteArray[0] = temp;
    return true;
}

/*user专用加密算法，DES算法*/
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


//简单解密算法，字符串逆移一位
bool CSerialPort::UnEntrypt(BYTE* byteArray, int length)
{
    BYTE temp = byteArray[0];
    for (int i = 0; i < length - 1; i++)
        byteArray[i] = byteArray[i + 1];
    byteArray[length - 1] = temp;
    return true;
}

/*user专用加解密函数, flag = 1为加密， flag = 2为解密*/
bool CSerialPort::entrypt(BYTE(&byteArray)[8], int num)
{
    miyao();
    string temp, str[2];
    for (int i = 0; i < 8; i++) temp += hex_to_binary(byteArray[i]);
    str[0] = temp;

    flag = num;
    temp = wen(str, 1);			//解密后二进制字符串
    
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






