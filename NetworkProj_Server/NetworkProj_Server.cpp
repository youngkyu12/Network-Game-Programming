#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;


int main()
{
	// 윈속 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	//윈속 종료
	WSACleanup();
}


