#ifndef _Tools_H
#define _Tools_H

#include <string>
using namespace std;

//��ȡ��ǰʱ�䣬��ʽΪ ʱ���֣��� ����
string  GetTime();

//��ȡ��ǰʱ�䣬��ʽΪ �꣺�£��գ�ʱ���֣���
string  GetFullTime();

//���������Ϣ
void getNetworkInfo();

//��ȡlinux mac��ַ
int GetMac(unsigned char* mac);

//�ж��Ƿ���ʮ������
bool IsHex(string input);

//�ж��Ƿ��Ƕ�����
bool IsBinary(string input);

//ɾȥstring�еĿո�
string trim_blank(string str);

//�ж�EPC�ַ����Ƿ�����ʱ��+0+����
bool isEPCPattern(string EPCString);

//�ж�Data�ַ����Ƿ�����MAC��ַ+0+����
bool isDataPattern(string DataString);


#endif
