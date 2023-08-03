#ifndef _ConvertData_H
#define	_ConvertData_H

#include <string>
using namespace std;

typedef unsigned char BYTE;

string IntToString(int data, int len);

int HexToInt(char c);
char IntToHex(int i);

//十六进制字符串 转 字节数组
BYTE* HexStringToByteArray(string hexStr, int start, int length);
BYTE* HexStringToByteArray(string hexStr);

//字节数组 转 十六进制
string ByteArrayToString(BYTE* byteArray, int start, int length);

//十六进制字节数组 转 二进制字符串
string hex_to_binary(BYTE b);

//二进制字符串
BYTE* BinaryStringToByteArray(string bStr, int start, int length);
BYTE* BinaryStringToByteArray(string bStr);


#endif // !_ConvertData_H

