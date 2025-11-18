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
	UPDATE = 3,


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

struct PlayerState // 한명분 데이터
{
	int32_t playerID;
	float x;
	float y;
	float z;
	uint16_t hp;
};

struct UpdateState
{
	Packetheader header;
	int32_t numPlayers;
	PlayerState players[3];
};
#pragma pack(pop)

//---------------------------

DWORD WINAPI RecvThreadMain(LPVOID lpParam)
{
	SOCKET sock = (SOCKET)lpParam;
	char recvBuffer[2048];
	while (1)
	{
		int recvBytes = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
		if (recvBytes > 0)
		{
			Packetheader* header = (Packetheader*)recvBuffer;

			if (header->ID == UPDATE)
			{
				UpdateState* updatePkt = (UpdateState*)recvBuffer;

				cout << "[서버 수신]" << updatePkt->numPlayers << "명" << endl;

				// 받은 모든 플레이어 정보 출력
				for (int i = 0; i < updatePkt->numPlayers; ++i)
				{
					cout << " ID " << updatePkt->players[i].playerID << " HP:" << updatePkt->players[i].hp << " X:" << updatePkt->players[i].x << " Y:" << updatePkt->players[i].y << ", Z : " << updatePkt->players[i].z << endl;
				}
			}
			else
			{
				cout << "알 수 없는 패킷" << endl;
			}
		}
		else if (recvBytes == 0)
		{
			cout << "서버 접속 종료" << endl;
			break;
		}
		else
		{
			break;
		}
	}
	return 0;
}

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

	CreateThread(NULL, 0, RecvThreadMain, (LPVOID)sock, 0, NULL);
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

		//cout << "보낸 바이트 크기 " << testPkt.header.size << endl;
		cout << "MOVE 패킷 전송 성공 x=" << testPkt.x << " y=" << testPkt.y << " z=" << testPkt.z << endl;
		Sleep(5000);

	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

