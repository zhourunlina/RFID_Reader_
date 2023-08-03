#include "ConvertData.h"
#include "Tools.h"
#include <iostream>
using namespace std;

//²»×ãlenÇ°ÖÃ0²¹×ã
string IntToString(int data, int len)
{
	string str = to_string(data);
	str = string(len - str.length(), '0') + str;
	return str;
}

int HexToInt(char c)
{
	int num;
	switch (c)
	{
	case '0': num = 0; break;
	case '1': num = 1; break;
	case '2': num = 2; break;
	case '3': num = 3; break;
	case '4': num = 4; break;
	case '5': num = 5; break;
	case '6': num = 6; break;
	case '7': num = 7; break;
	case '8': num = 8; break;
	case '9': num = 9; break;
	case 'A': num = 10; break;
	case 'B': num = 11; break;
	case 'C': num = 12; break;
	case 'D': num = 13; break;
	case 'E': num = 14; break;
	case 'F': num = 15; break;
	}
	return num;
}

char IntToHex(int i)
{
	char c = ' ';
	switch (i)
	{
	case 0: c = '0'; break;
	case 1: c = '1'; break;
	case 2: c = '2'; break;
	case 3: c = '3'; break;
	case 4: c = '4'; break;
	case 5: c = '5'; break;
	case 6: c = '6'; break;
	case 7: c = '7'; break;
	case 8: c = '8'; break;
	case 9: c = '9'; break;
	case 10: c = 'A'; break;
	case 11: c = 'B'; break;
	case 12: c = 'C'; break;
	case 13: c = 'D'; break;
	case 14: c = 'E'; break;
	case 15: c = 'F'; break;
	}
	return c;
}

BYTE* HexStringToByteArray(string hexStr, int start, int length)
{
	hexStr = trim_blank(hexStr);
	if (!IsHex(hexStr)) return NULL;
	int hexLen = hexStr.length() / 2;
	hexLen -= start;
	hexLen = hexLen > length ? length : hexLen;
	BYTE* bytResult = new BYTE[hexLen];
	for (int i = start; i < hexLen; i++)
	{
		bytResult[i] = (BYTE)(HexToInt(hexStr[2 * i]) * 16 + HexToInt(hexStr[2 * i + 1]));
	}
	return bytResult;
}

BYTE* HexStringToByteArray(string hexStr)
{
	return HexStringToByteArray(hexStr, 0, hexStr.length() / 2);
}

string ByteArrayToString(BYTE* byteArray, int start, int length)
{
	if (byteArray == NULL)
		return "";
	string str;
	for (int i = 0; i < length; i++)
	{
		int high = (byteArray[i] >> 4) & 0x0F, low = byteArray[i] & 0x0F;
		str = str + IntToHex(high) + IntToHex(low);
	}
	return str;
}

string hex_to_binary(BYTE b)
{
	string res = "";
	for (int i = 7; i >= 0; i--)  res += (b >> i & 1) + '0';
	return res;
}


BYTE* BinaryStringToByteArray(string bStr, int start, int length)
{
	bStr = trim_blank(bStr);
	if (!IsBinary(bStr)) return NULL;
	int bLen = bStr.length() / 8;
	bLen -= start;
	bLen = bLen > length ? length : bLen;
	BYTE* bytResult = new BYTE[bLen];
	for (int i = start; i < bLen; i ++)
	{
		int high = (bStr[8 * i] - '0') * 8 + (bStr[8 * i + 1] - '0') * 4 + (bStr[8 * i + 2] - '0') * 2 + (bStr[8 * i + 3] - '0');
		int low = (bStr[8 * i + 4] - '0') * 8 + (bStr[8 * i + 5] - '0') * 4 + (bStr[8 * i + 6] - '0') * 2 + (bStr[8 * i + 7] - '0');
   
		bytResult[i] = (BYTE)(high * 16 + low);
	}
	return bytResult;
}

BYTE* BinaryStringToByteArray(string bStr)
{
	return BinaryStringToByteArray(bStr, 0, bStr.length() / 8);
}