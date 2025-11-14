#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

#define SERVERADDR "127.0.0.1"
#define SERVERPORT 8922


//--------------------------

#pragma pack(push, 1)
enum
{
	MOVE = 1,


};
struct Packetheader
{
	uint16_t size;
	uint16_t ID; // 프로토콜 ID

};
struct MovePacket
{
	Packetheader header;
	float x;
	float y;
	float z;
};
#pragma pack(pop)

//---------------------------

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
	inet_pton(AF_INET, SERVERADDR, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);

	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return 1;
	}
	cout << "연결 성공" << endl;

	while (true)
	{
		MovePacket testPkt;

		testPkt.header.ID = MOVE;
		testPkt.header.size = sizeof(MovePacket); // 16바이트

		
		testPkt.x = 10.5f;
		testPkt.y = 0.0f;
		testPkt.z = -20.1f;


		if (send(sock, (char*)&testPkt, testPkt.header.size, 0) == SOCKET_ERROR)
		{
			cout << "전송 실패" << endl;
			break;
		}

		cout << "보낸 바이트 크기 " << testPkt.header.size << endl;
		cout << "MOVE 패킷 전송 성공 x=" << testPkt.x << " y=" << testPkt.y << " z=" << testPkt.z << endl;
		Sleep(5000);
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

