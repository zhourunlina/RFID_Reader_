#ifndef _Tools_H
#define _Tools_H
#include <string>
#include <regex>
using namespace std;

//��ȡ��ǰʱ�䣬��ʽΪ ʱ���֣��� ����
string  getTime();

//��ȡlinux mac��ַ
int getMac(unsigned char* mac);

//�ж�EPC�ַ����Ƿ�����ʱ��+0+����
bool is_epc_pattern(string epc);

//�ж�USER�ַ����Ƿ�����MAC��ַ+0+����
bool is_user_pattern(string user);

//�ж��Ƿ���ʮ������
bool is_hex(string str);

//�ж��Ƿ��Ƕ�����
bool is_binary(string str);

//ɾȥstring�еĿո�
string trim_blank(string str);

#endif