#ifndef _ConvertData_H
#define	_ConvertData_H

#include <string>
using namespace std;

typedef unsigned char BYTE;

string int_to_str(int data, int len);

//�ֽ����� ת ʮ������
string byte_array_to_string(BYTE* byteArray, int start, int length);

//�������ַ��� ת �ֽ�����
BYTE* bstring_to_byte_array(string bStr, int start, int length);
BYTE* bstring_to_byte_array(string bStr);

int hex_to_int(char c);
char int_to_hex(int i);

string hex_to_binary(BYTE b);

#endif 
