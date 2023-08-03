#include "ConvertData.h"
#include "Tools.h"
#include <chrono>
#include <regex>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>


string  GetTime()
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

	string time = IntToString(time_tm->tm_hour, 2) + ":" + IntToString(time_tm->tm_min, 2) + ":" + IntToString(time_tm->tm_sec, 2)
		+ " " + IntToString((int)dis_millseconds, 3);
	return time;
}

string  GetFullTime()
{
	//获取当前时间
	auto nowtime = chrono::system_clock::now();

	//转化为time_t
	time_t tt = chrono::system_clock::to_time_t(nowtime);

	//格式化时间
	tm* time_tm = localtime(&tt);
	string time = IntToString(time_tm->tm_year + 1900, 4) + IntToString(time_tm->tm_mon + 1, 2) + IntToString(time_tm->tm_mday, 2) +
		IntToString(time_tm->tm_hour, 2) + IntToString(time_tm->tm_min, 2) + IntToString(time_tm->tm_sec, 2);
	return time;
}

void getNetworkInfo()
{
    // get hostname and external ip, simple and best way
    char host_name[256] = { 0 };
    gethostname(host_name, sizeof(host_name));

    struct hostent* host = gethostbyname(host_name);
    char ip_str[32] = { 0 };
    const char* ret = inet_ntop(host->h_addrtype, host->h_addr_list[0], ip_str, sizeof(ip_str));
    if (ret != NULL)
    {
        printf("Current HostName: %s\n", host_name);
        printf("Current external IP: %s\n", ip_str);
    }

    // get multiple ip, works for linux
    struct ifaddrs* ifAddrStruct = NULL;
    struct ifaddrs* ifa = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;

        // check if IP4
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            char local_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, local_ip, INET_ADDRSTRLEN);

            // actually only need external ip
            printf("%s IP: %s\n", ifa->ifa_name, local_ip);
        }
    }

    if (ifAddrStruct)
        freeifaddrs(ifAddrStruct);


    // get mac, need to create socket first, may not work for mac os
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    char local_mac[128] = { 0 };

    strcpy(ifr.ifr_name, "eth0"); // only need ethernet card
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
    {
        char temp_str[10] = { 0 };
        memcpy(temp_str, ifr.ifr_hwaddr.sa_data, 6);
        sprintf(local_mac, "%02x-%02x-%02x-%02x-%02x-%02x", temp_str[0] & 0xff, temp_str[1] & 0xff, temp_str[2] & 0xff, temp_str[3] & 0xff, temp_str[4] & 0xff, temp_str[5] & 0xff);
    }

    printf("Local Mac: %s\n", local_mac);
}

int GetMac(unsigned char* mac)
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

bool IsHex(string input)
{
	string pattern = "^[A-Fa-f0-9]+$";
	regex hex(pattern);
	return regex_match(input, hex);
}

bool IsBinary(string input)
{
    string pattern = "^[0-1]+$";
    regex binary(pattern);
    return regex_match(input, binary);
}

string trim_blank(string str)
{
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}

bool isEPCPattern(string EPCString)
{
    string pattern_prefix = "^\\d{14}[0]{8}$";
    regex prefix(pattern_prefix);
    return regex_match(EPCString.substr(0, 22), prefix) && IsHex(EPCString.substr(22, 24));
}

bool isDataPattern(string DataString)
{
    unsigned char mac[10];
    GetMac(mac);
    string macAddr = ByteArrayToString(mac, 0, 6);

    if (!(DataString.substr(0, 12) == macAddr))
        return false;

    return IsHex(DataString.substr(12, 4));
}