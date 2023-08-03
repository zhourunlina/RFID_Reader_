#ifndef _Tools_H
#define _Tools_H
#include <string>
#include <regex>
using namespace std;

//获取当前时间，格式为 时：分：秒 毫秒
string  getTime();

//获取linux mac地址
int getMac(unsigned char* mac);

//判断EPC字符串是否满足时间+0+次数
bool is_epc_pattern(string epc);

//判断USER字符串是否满足MAC地址+0+次数
bool is_user_pattern(string user);

//判断是否是十六进制
bool is_hex(string str);

//判断是否是二进制
bool is_binary(string str);

//删去string中的空格
string trim_blank(string str);

#endif