#ifndef _DES_H
#define _DES_H
#include <iostream>
using namespace std;

//字符转二进制，会有些怪异，但是后面用binaryToInt函数解决
string charToBinary(char c);

//二进制转整型
int binaryToInt(string s);

//整型转二进制，用来将进入S盒后取出的数据转化为二进制，最多用4bit(while循环中4的出处)
string intToBinary(int i);

//异或运算
string xorAB(string a, string b);

//f函数
string f(string right, string k);

//明文/密文处理,以明文处理过程为例进行解释
string wen(string wenBinary[], int num);

//密钥处理
void miyao();

#endif
