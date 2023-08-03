#ifndef _ConvertData_H
#define	_ConvertData_H

#include <string>
using namespace std;

typedef unsigned char BYTE;

string int_to_str(int data, int len);

//字节数组 转 十六进制
string byte_array_to_string(BYTE* byteArray, int start, int length);

//二进制字符串 转 字节数组
BYTE* bstring_to_byte_array(string bStr, int start, int length);
BYTE* bstring_to_byte_array(string bStr);

int hex_to_int(char c);
char int_to_hex(int i);

string hex_to_binary(BYTE b);

#endif 
