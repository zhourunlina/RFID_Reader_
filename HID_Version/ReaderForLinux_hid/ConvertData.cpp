#include "ConvertData.h"
#include "Tools.h"
#include <iostream>
using namespace std;

/*int×ªstring£¬²»×ãlenÇ°ÖÃ0²¹×ã*/
string int_to_str(int data, int len)
{
	string str = to_string(data);
	str = string(len - str.length(), '0') + str;
	return str;
}


string byte_array_to_string(BYTE* byteArray, int start, int len)
{
	if (byteArray == NULL)	return "";

	string str;
	for (int i = 0; i < len; i++)
	{
		int high = (byteArray[i] >> 4) & 0x0F, low = byteArray[i] & 0x0F;
		str += int_to_hex(high);
		str += int_to_hex(low);
	}
	return str;
}

BYTE* bstring_to_byte_array(string bStr, int start, int len)
{
	bStr = trim_blank(bStr);
	if (!is_binary(bStr)) return NULL;
	int bLen = bStr.length() / 8;
	bLen -= start;
	bLen = bLen > len ? len : bLen;
	BYTE* bytResult = new BYTE[bLen];
	for (int i = start; i < bLen; i++)
	{
		int high = (bStr[8 * i] - '0') * 8 + (bStr[8 * i + 1] - '0') * 4 + (bStr[8 * i + 2] - '0') * 2 + (bStr[8 * i + 3] - '0');
		int low = (bStr[8 * i + 4] - '0') * 8 + (bStr[8 * i + 5] - '0') * 4 + (bStr[8 * i + 6] - '0') * 2 + (bStr[8 * i + 7] - '0');

		bytResult[i] = (BYTE)(high * 16 + low);
	}
	return bytResult;
}

BYTE* bstring_to_byte_array(string bStr)
{
	return bstring_to_byte_array(bStr, 0, bStr.length() / 8);
}

int hex_to_int(char c)
{
	switch (c)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': return 10;
	case 'B': return 11;
	case 'C': return 12;
	case 'D': return 13;
	case 'E': return 14;
	case 'F': return 15;
	}
}

char int_to_hex(int i)
{
	switch (i)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'A';
	case 11: return 'B';
	case 12: return 'C';
	case 13: return 'D';
	case 14: return 'E';
	case 15: return 'F';
	}
}

string hex_to_binary(BYTE b)
{
	string res = "";
	for (int i = 7; i >= 0; i--)  res += (b >> i & 1) + '0';
	return res;
}