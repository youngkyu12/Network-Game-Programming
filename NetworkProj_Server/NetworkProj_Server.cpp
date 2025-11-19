#include "pch.h"
#include "GameRoom.h"
#define SERVERPORT 8922

GameRoom Room;

/*
recvData는 데이터를 받을 위치를 계산한 포인터 변수 입니다.
가독성을 위한 일종의 책갈피 역할이라고 생각해주시면 됩니다.
*/

DWORD WINAPI WorkerThreadMain(LPVOID lpParam)
{
	Player* myPlayer = (Player*)lpParam;
	//Room.UpdatePlayer(myPlayer->Player_ID);
	myPlayer->SetPosition(0, 0, -50 + (myPlayer->Player_ID * 100));
	while (true)
	{
		Room.Update_State(myPlayer);

		// 시작주소 갱신.
		char* recvData = (char*)myPlayer->recvBuffer + myPlayer->recvByte;// 포인터 연산으로 (타입)*크기 만큼 이동
		cout << "ID : " << myPlayer->Player_ID << "리시브 데이터 크기" << myPlayer->recvByte << endl;

		int32_t remainSize = BUF_SIZE - myPlayer->recvByte;
		if (remainSize <= 0)
		{
			//남은 공간 없으면 비정상 종료 처리.
			cout << myPlayer->Player_ID << " 수신버퍼 오버플로우" << endl;
			Room.Remove_Player(myPlayer);
			Room.Check_PLayer();
			break;
		}
		
		int recvBytes = recv(myPlayer->sock, recvData, remainSize, 0);//버퍼에 남은 공간만큼만 받기
		if (recvBytes > 0)
		{
			myPlayer->recvByte += recvBytes; //버퍼에 쌓인 데이터 크기 갱신
			cout << "ID " << myPlayer->Player_ID << " :" << recvBytes << endl;

			BYTE* Tempbuffer = myPlayer->recvBuffer;
			while (1)
			{
				if (myPlayer->recvByte < sizeof(Packetheader))
				{
					// 패킷헤더만큼도 도착안했으면
					cout << "헤더보다 작음" << endl;
					break;
				}
				Packetheader* header = (Packetheader*)Tempbuffer;

				if (myPlayer->recvByte < header->size)
				{
					//패킷이 아직 다 안옴.
					cout << "패킷 아직 다 도착X" << endl;
					break;
				}
				// 완벽히 도착한 패킷은 HandlePacket에서 처리.
				Room.HandlePacket(myPlayer, Tempbuffer);

				// 처리한 패킷만큼 버퍼에서 정리.
				if ((myPlayer->recvByte) - (header->size) > 0)
				{
					// 남은 데이터를 버퍼 맨 앞으로 당기기.
					memmove(Tempbuffer, Tempbuffer + header->size, (myPlayer->recvByte) - (header->size));
				}
				myPlayer->recvByte = (myPlayer->recvByte) - (header->size);// 실제 버퍼 총 크기 갱신.
			}
		}

		else if (recvBytes == 0)
		{
			//정상 종료
			cout << "ID : " << myPlayer->Player_ID << " 접속 종료" << endl;
			break;
		}
		else if (recvBytes == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				//오류가 아닌 의도한 것
				cout << "recv 500ms 타임아웃" << endl;
				continue;
			}
			
			//비정상 종료
			cout << "ID : " << myPlayer->Player_ID << " 비정상 종료" << endl;
			Room.Remove_Player(myPlayer);
			Room.Check_PLayer();
			break;
		}
	}
	
	closesocket(myPlayer->sock);
	delete myPlayer;
	return 0;
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
		
		//recv 100ms 동안 안들어오면 타임아웃오류
		int recvTimeout = 33; //100ms
		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recvTimeout, sizeof(recvTimeout)) == SOCKET_ERROR)
		{
			cout << "옵션 설정 실패" << endl;
			closesocket(clientSocket);
			return 1;
		}

		// 힙영역에서 모든 플레이어 객체 관리
		Player* player = new Player;
		player->sock = clientSocket;
		//Player_ID 부여
		player->Player_ID = ID_NUM++;
		cout << "연결 성공" << endl;
		cout << "Player ID : " << player->Player_ID << endl;

		Room.Add_Player(player);
		// 접속한 플레이어 인원 수
		Room.Check_PLayer();

		// 쓰레드 생성
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


