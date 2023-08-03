#include "Tools.h"
#include "ConvertData.h"
#include <chrono>
#include <ifaddrs.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <iostream>
using namespace std;

string  getTime()
{
	//获取当前时间
	auto nowtime = chrono::system_clock::now();
	//通过不同精度获取相差的毫秒数
	uint64_t dis_millseconds = chrono::duration_cast<chrono::milliseconds>(nowtime.time_since_epoch()).count()
		- chrono::duration_cast<chrono::seconds>(nowtime.time_since_epoch()).count() * 1000;

	//转化为time_t
	time_t tt = chrono::system_clock::to_time_t(nowtime);

	//格式化时间
	tm* time_tm = localtime(&tt);

	string time = int_to_str(time_tm->tm_hour, 2) + ":" + int_to_str(time_tm->tm_min, 2) + ":" + int_to_str(time_tm->tm_sec, 2)
		+ " " + int_to_str((int)dis_millseconds, 3);
	return time;
}

int getMac(unsigned char* mac)
{
	// get mac, need to create socket first, may not work for mac os
	struct ifreq ifr;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	strcpy(ifr.ifr_name, "eth0"); // only need ethernet card
	if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
	{
		char temp_str[10] = { 0 };
		memcpy(temp_str, ifr.ifr_hwaddr.sa_data, 6);
		for (int i = 0; i < 6; i++)
			mac[i] = temp_str[i] & 0xff;
	}

	return 0;
}

bool is_epc_pattern(string epc)
{
	string pattern_prefix = "^\\d{14}[0]{8}$";
	regex prefix(pattern_prefix);
	return regex_match(epc.substr(0, 22), prefix) && is_hex(epc.substr(22, 24));
}

bool is_user_pattern(string user)
{
	BYTE mac[10];
	getMac(mac);
	string macAddr = byte_array_to_string(mac, 0, 6);

	return (user.substr(0, 12) == macAddr) && is_hex(user.substr(12, 4));
}

bool is_hex(string str)
{
	string pattern = "^[A-Fa-f0-9]+$";
	regex hex(pattern);
	return regex_match(str, hex);
}

bool is_binary(string str)
{
	string pattern = "^[0-1]+$";
	regex binary(pattern);
	return regex_match(str, binary);
}

string trim_blank(string str)
{
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}