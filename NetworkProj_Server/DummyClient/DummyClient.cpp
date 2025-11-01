#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		return 1;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(8922);

	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return 1;
	}
	cout << "연결 성공" << endl;

	char buf[1024];
	const char* testMessage = "###$%##클라이언트로 부터 온 메세지 입니다.###$$%@%@";
	int msgLen = strlen(testMessage) + 1;// 널문자포함
	while (true)
	{
		if (send(sock, testMessage, msgLen, 0) == SOCKET_ERROR)
		{
			break;
		}
		cout << "메세지 전송 성공" << endl;
		Sleep(1000);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}

