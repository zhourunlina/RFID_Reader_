#ifndef _Tools_H
#define _Tools_H

#include <string>
using namespace std;

//获取当前时间，格式为 时：分：秒 毫秒
string  GetTime();

//获取当前时间，格式为 年：月：日：时：分：秒
string  GetFullTime();

//获得网络信息
void getNetworkInfo();

//获取linux mac地址
int GetMac(unsigned char* mac);

//判断是否是十六进制
bool IsHex(string input);

//判断是否是二进制
bool IsBinary(string input);

//删去string中的空格
string trim_blank(string str);

//判断EPC字符串是否满足时间+0+次数
bool isEPCPattern(string EPCString);

//判断Data字符串是否满足MAC地址+0+次数
bool isDataPattern(string DataString);


#endif
