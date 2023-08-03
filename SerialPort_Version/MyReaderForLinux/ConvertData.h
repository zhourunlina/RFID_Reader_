#ifndef _ConvertData_H
#define	_ConvertData_H

#include <string>
using namespace std;

typedef unsigned char BYTE;

string IntToString(int data, int len);

int HexToInt(char c);
char IntToHex(int i);

//ʮ�������ַ��� ת �ֽ�����
BYTE* HexStringToByteArray(string hexStr, int start, int length);
BYTE* HexStringToByteArray(string hexStr);

//�ֽ����� ת ʮ������
string ByteArrayToString(BYTE* byteArray, int start, int length);

//ʮ�������ֽ����� ת �������ַ���
string hex_to_binary(BYTE b);

//�������ַ���
BYTE* BinaryStringToByteArray(string bStr, int start, int length);
BYTE* BinaryStringToByteArray(string bStr);


#endif // !_ConvertData_H

