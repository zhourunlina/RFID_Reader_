#ifndef _DES_H
#define _DES_H
#include <iostream>
using namespace std;

//�ַ�ת�����ƣ�����Щ���죬���Ǻ�����binaryToInt�������
string charToBinary(char c);

//������ת����
int binaryToInt(string s);

//����ת�����ƣ�����������S�к�ȡ��������ת��Ϊ�����ƣ������4bit(whileѭ����4�ĳ���)
string intToBinary(int i);

//�������
string xorAB(string a, string b);

//f����
string f(string right, string k);

//����/���Ĵ���,�����Ĵ������Ϊ�����н���
string wen(string wenBinary[], int num);

//��Կ����
void miyao();

#endif
