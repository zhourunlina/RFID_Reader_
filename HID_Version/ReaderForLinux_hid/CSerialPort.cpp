#include "CSerialPort.h"
#include <cstring>
#include <sys/ioctl.h>
using namespace std;

CSerialPort::CSerialPort()
{
    fd = -1, port_addr = 65535, sendbuff = NULL;
}

CSerialPort::~CSerialPort(){}

/*
    dev:串口名称， 
    baudrate：串口支持的比特率
    flow_ctrl：数据流控制
    databits：数据位
    stopbits：停止位
    parity：校验位
*/
bool CSerialPort::open_and_setup(const char* dev, int baudrate, int flow_ctrl, int databits, int stopbits, int parity)
{
    if (!open_port(dev)) return false;

    if (!setup(57600, 0, 8, 1, 'N')) return false;

    return true;
}

bool CSerialPort::open_port(const char* dev)
{
    char* _dev = new char[32];
    strcpy(_dev, dev);

    fd = open(_dev, O_RDWR);            //O_RDWR 支持读写| O_NOCTTY | O_NDELAY   
    if (-1 == fd)   return false;

    int DTR_flag = TIOCM_DTR;

    ioctl(fd, TIOCMBIS, &DTR_flag);    //Set RTS pin

    return true;
}

bool CSerialPort::setup(int baudrate, int flow_ctrl, int databits, int stopbits, int parity)
{
    int   status;

    /*
    tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1, 串口调用失败");
        return false;
    }

    /*设置串口输入波特率和输出波特率*/
    for (int i = 0; i < sizeof(baudrate_arr) / sizeof(int); i++)
    {
        if (baudrate == name_arr[i])
        {
            cfsetispeed(&options, baudrate_arr[i]);
            cfsetospeed(&options, baudrate_arr[i]);
            break;
        }
    }

    options.c_cflag |= CLOCAL;  //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CREAD;   //修改控制模式，使得能够从串口中读取输入数据

    /*数据流控制*/
    switch (flow_ctrl)  
    {
    case 0: options.c_cflag &= ~CRTSCTS; break;                 //不使用流控制  
    case 1: options.c_cflag |= CRTSCTS; break;                  //使用硬件流控制 
    case 2: options.c_cflag |= IXON | IXOFF | IXANY;            //使用软件流控制  
    }
  
    options.c_cflag &= ~CSIZE;  //屏蔽其他标志位

    /*数据位控制*/
    switch (databits)        
    {
    case 5: options.c_cflag |= CS5; break;
    case 6: options.c_cflag |= CS6; break;
    case 7: options.c_cflag |= CS7; break;
    case 8: options.c_cflag |= CS8; break;
    default: 
        fprintf(stderr, "Unsupported data size\n"); 
        return false;
    }

    /*停止位控制*/
    switch (stopbits) 
    {
    case 1: options.c_cflag &= ~CSTOPB; break;
    case 2: options.c_cflag |= CSTOPB; break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return false;
    }

    /*校验位控制*/
    switch (parity)  
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


    options.c_oflag &= ~OPOST;  //修改输出模式，原始数据输出
    
    //修改输入模式, 解决特殊字符丢失问题, 如0D强制变0A， 接收不到0x11, 0x0d,0x13
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);  


    /*设置等待时间和最小接收字符*/ 
    options.c_cc[VTIME] = 10;               //读取一个字符等待10*(1/10)s 
    options.c_cc[VMIN] = 50;                //读取字符的最少个数为50 
     
    tcflush(fd, TCIFLUSH);                  //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读 

    /*激活配置 (将修改后的termios数据设置到串口中）*/  
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("com set error!\n");
        return false;
    }

    return true;
}

void CSerialPort::close_port()
{
    close(fd);
}


int CSerialPort::send(vector<BYTE> byteArray)
{
    sendbuff = new BYTE[byteArray.size()];
    for (int i = 0; i < byteArray.size(); i++)
        sendbuff[i] = byteArray[i];
    return write(fd, sendbuff, byteArray.size());
}
int CSerialPort::send(ProtocolPacket_RFID protocolPacket)
{
    protocolPacket.show_protocol_packet();
    return send(protocolPacket.to_byte_array());
}
int CSerialPort::send(ProtocolPacket_TMCM protocolPacket)
{
    protocolPacket.show_protocol_packet();
    return send(protocolPacket.to_byte_array());
}
int CSerialPort::send(ProtocolPacket_XMT protocolPacket)
{
    protocolPacket.show_protocol_packet();
    return send(protocolPacket.to_byte_array());
}

int CSerialPort::recv_for_auto(BYTE recvByteArray[], int length)
{
    return read(fd, recvByteArray, length);    //read函数返回的字符数是实际串口收到的字符数
}

int CSerialPort::recv_for_auto_tl(BYTE recvByteArray[], int length, int time)
{
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    //FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);

    // Set timeout to 1.0 seconds
    struct timeval timeout;
    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    if (select(fd + 1, &read_fds, NULL, &except_fds, &timeout))
    {
        return read(fd, recvByteArray, length);
    }
    else return -2;
}

/*应答模式对返回的包进行拆包*/
void CSerialPort::divided_rtn_packet(BYTE byteArray[], int length)
{
    if (length < 7)  return;

    int num;
    for (num = 0; num < length; num++)
    {
        if (byteArray[num] != 3)    continue;       //找到第一个03，即应答的起始位标志

        /*
        num + 5 为Length，Length特指INFO的长度；
        协议规定命令包最小为7字节，包括：起始位（1）+ 地址（2）+ CID1（1）+ CID2（1）+ Length（1）+ INFO（>=1） + CheckSum（1）
        */
        if (length - num < 7 + byteArray[num + 5])  return;

        /*num + 1 为低地址， num + 2 为高地址， |为位运算的或运算*/
        int addr = byteArray[num + 1] | (byteArray[num + 2] << 8);

        /*CID2 在回答中表现为返回码 RTN ，00为正常，01为错误，02位命令返回卡号，05为主动上送卡号*/
        BYTE rtn = (BYTE)(byteArray[num + 4] & 0x7f);
        if (rtn == 0 || rtn == 1 || rtn == 2 || rtn == 5)
        {
            if (port_addr != addr) port_addr = addr;
            break;
        }
    }

    
    length -= num;                                //删除起始位标志前的字符消息,以num作为有效信息的第一位
    vector<BYTE> cmdByteArray;

    while (length >= 7)
    {
        int cmdLen = 7 + byteArray[num + 5];      ////byteArray[num + 5]即infoLen，即返回Info（DATA）的长度;
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


int CSerialPort::get_dev_info()
{
    return send(ProtocolPacket_RFID(this->port_addr, GET_INFO, MSG_GET));
}

int CSerialPort::get_epc()
{
    return send(ProtocolPacket_RFID(this->port_addr, READ_EPC_ONLY, MSG_CMD));
}

int CSerialPort::get_data(BYTE membank)
{
    vector<BYTE> param;
    
    for (int i = 0; i < 4; i++) param.push_back(0x00);  //accPSW 确认密码，缺省值为 00 00 00 00；
    
    param.push_back(membank);                           //存储区域

    BYTE startAddr, dataLen;
    switch (membank)
    {
    case 0: startAddr = 0x00; dataLen = 0x04; break;    //RFU
    case 1: startAddr = 0x02; dataLen = 0x06; break;    //EPC
    case 2: startAddr = 0x00; dataLen = 0x06; break;    //TID
    case 3: startAddr = 0x00; dataLen = 0x04; break;    //USER
    }
    param.push_back(startAddr);                         //起始地址
    param.push_back(dataLen);                           //读取数据长度

    return send(ProtocolPacket_RFID(this->port_addr, READ_EPC_AND_DATA, MSG_CMD, param));
}

int CSerialPort::set_data(BYTE membank, BYTE data[])
{
    vector<BYTE> param;

    for (int i = 0; i < 4; i++) param.push_back(0x00);  //accPSW 确认密码，缺省值为 00 00 00 00；

    param.push_back(membank);                           //存储区域

    BYTE startAddr, dataLen;
    switch (membank)
    {
    case 1: startAddr = 0x02; dataLen = 0x06; break;    //EPC
    case 3: startAddr = 0x00; dataLen = 0x04; break;    //USER
    }
    param.push_back(startAddr);                         //起始地址
    param.push_back(dataLen);                           //读取数据长度

    for (int i = 0; i < dataLen * 2; i++) param.push_back(data[i]);

    return send(ProtocolPacket_RFID(this->port_addr, WRITE_DATA, MSG_CMD, param));
}