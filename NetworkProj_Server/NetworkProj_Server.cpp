#include "pch.h"
#include "Player.h"
#include "GameRoom.h"


#define SERVERPORT 8922

DWORD WINAPI WorkerThreadMain(LPVOID lpParam)
{
	Player* myPlayer = (Player*)lpParam;
	
	while (true)
	{
		GameRoom::Update_State();
		//recv();
		//조건문으로 게임로직
	}
	
	closesocket(myPlayer->sock);
	delete myPlayer;
}

int main()
{
	// 윈속 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	// 소켓 생성
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		return 1;
	}

	// 주소, 포트 입력
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	// bind
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return 1;
	}

	// listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 1;
	}

	int ID_NUM = 0;
	while (true)
	{
		// 접속 시작
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			return 1;
		}

		// 힙영역에서 모든 플레이어 객체 관리
		Player* player = new Player;
		player->sock = clientSocket;
		//Player_ID 부여
		player->Player_ID = ID_NUM++;

		HANDLE WorkerThread = CreateThread(NULL, 0, WorkerThreadMain, player, 0, NULL);

		// 핸들을 종료
		if (WorkerThread)
		{
			CloseHandle(WorkerThread);
		}
	}
	// 리슨소켓 종료
	closesocket(listenSocket);
	//윈속 종료
	WSACleanup();
}


