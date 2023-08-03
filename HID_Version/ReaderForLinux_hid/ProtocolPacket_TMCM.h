#ifndef ProtocolPacket_TMCM_H
#define ProtocolPacket_TMCM_H

#include <vector>
using namespace std;

#define TMCL_ROR 1				//运动指令：右转
#define TMCL_ROL 2				//运动指令：左转
#define TMCL_MST 3				//运动指令：移动停止
#define TMCL_MVP 4				//运动指令：移动到指定位置
#define TMCL_SOC 30				//运动指令：存坐标
#define TMCL_GCO 31				//运动指令：捕获坐标(使用此命令可以读回先前存储的坐标)
#define TMCL_CCO 32				//运动指令：读取坐标
#define TMCL_RTPR 138           //设置指令：请求到达指定位置事件（使用此命令后MVP指令会在电机到达指定位置时额外回传一条信息）

#define TMCL_GIO 15             //

/*Options for MVP commands 移动操作的可选项*/
#define MVP_ABS 0				//绝对位移
#define MVP_REL 1				//相对位移
#define MVP_COORD 2				//移动到先前存储(SOC)的坐标

/*Options for RTPR commands 请求到达指定位置事件的可选项*/
#define RTPR_NEXT 0
#define RTPR_ALL 1

/*Options for GIO commands 可选项*/
#define DIGITAL_INPUT_1 0       // gpio 0
#define DIGITAL_INPUT_2 1       // gpio 1
#define DIGITAL_INPUT_3 2       // gpio 2
#define DIGITAL_INPUT_4 3       // gpio 3


//#define MVP_REL 1				//


typedef unsigned char BYTE;

class ProtocolPacket_TMCM
{
public:
	BYTE Address;				//地址
	BYTE Command;				//命令
	BYTE Type;					//类型
	BYTE Motor;					//轴
	BYTE Data[4];				//数值
	BYTE Checksum;				//检验和

    ProtocolPacket_TMCM();
    ProtocolPacket_TMCM(int command);
    ProtocolPacket_TMCM(int command, int type, int value);
    ProtocolPacket_TMCM(int command, int type, int motor,int value);
    ~ProtocolPacket_TMCM();

	BYTE checksum();

	void build_protocol_packet(BYTE nAddr, BYTE nCommand, BYTE nType, BYTE nMotor, vector<BYTE> ArgByte);

	void show_protocol_packet();

	vector<BYTE> to_byte_array();
};
#endif

