// LabProject02-01.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "2019180042_1.h"
#include "GameFramework.h"
#include "SocketQueue.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

CGameFramework		gGameFramework;

 char SeverIP[16];

 HANDLE hIpEvent; // connect 대기 이벤트
 HWND hEdit = nullptr;
 HANDLE hClientThread;

 void DisplayText(const char* fmt, ...);
 void DisplayError(const char* msg);
 void DisplayError_Quit(const char* msg);

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ClientMain(LPVOID arg);	// 소켓 통신 스레드 함수



int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	hIpEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY2019180042_1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);


	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2019180042_1));


	// 기본 메시지 루프입니다.
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gGameFramework.IsRunning())
			{
				gGameFramework.FrameAdvance();
			}
		}
	}

	if (hClientThread != NULL)
	{
		WaitForSingleObject(hClientThread, INFINITE);// 스레드 종료 대기
		CloseHandle(hClientThread);

	}
	gGameFramework.OnDestroy();

	// 이벤트 제거
	CloseHandle(hIpEvent);

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2019180042_1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rc = { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}




//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static HWND hIPControl = nullptr;
	static HWND hButton = nullptr;
	
	switch (message)
	{
	case WM_CREATE:
		// StartScene
		hIPControl = CreateWindow(WC_IPADDRESS, NULL,
			WS_CHILD | WS_VISIBLE,
			20, 220, 200, 25,
			hWnd, (HMENU)ID_IPADDRESS,
			hInst, NULL);
		SendMessage(hIPControl, IPM_SETADDRESS, 0, MAKEIPADDRESS(127,0,0,1));
		// button
		hButton = CreateWindow(_T("button"), _T("접속"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			240, 220, 80, 25,
			hWnd, (HMENU)ID_CONNECT_BUTTON,
			hInst, NULL);
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (gGameFramework.IsRunning())
		{
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴의 선택 영역을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_CONNECT_BUTTON:
		{
			DWORD ip;
			SendMessage(hIPControl, IPM_GETADDRESS, 0, (LPARAM)&ip);

			BYTE b1 = FIRST_IPADDRESS(ip);
			BYTE b2 = SECOND_IPADDRESS(ip);
			BYTE b3 = THIRD_IPADDRESS(ip);
			BYTE b4 = FOURTH_IPADDRESS(ip);

			char ipStr[16];
			sprintf(ipStr, "%d.%d.%d.%d", b1, b2, b3, b4);

			// 예시: 출력 확인
			MessageBoxA(hWnd, ipStr, "입력한 IP", MB_OK);
			strcpy(SeverIP, ipStr);
			SetEvent(hIpEvent);
			gGameFramework.SetRunning(true);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		hEdit = GetDlgItem(hDlg, IDC_EDIT);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	int num = 0;
	
	WaitForSingleObject(hIpEvent, INFINITE); // Server IP 작성 완료 대기
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		DisplayError_Quit("socket()");
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, SeverIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);

	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		DisplayError_Quit("connect()");
	}
	
	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	int len = 0;
	int32_t recvByte = 0;

	// 서버와 데이터 통신
	while (gGameFramework.IsRunning())
	{
		//버퍼를 계속 덮어쓰기로 저장하게 해서 쪼개져서 오면 제대로 수신을 못하고 있어 수정했습니다.
		char* recvData = buf + recvByte;

		retval = recv(sock, recvData, (sizeof(buf) - recvByte), 0);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
			}
			else
			{
				break;
			}
		}
		else if (retval == 0)
			break;
		if (retval > 0)
		{
			recvByte += retval;
			while (1)
			{
				if (recvByte < sizeof(Packetheader))
				{
					// 패킷헤더만큼도 도착안함
					break;
				}

				Packetheader* header = (Packetheader*)buf;

				if (recvByte < header->size)
				{
					// 패킷이 아직 다 안옴.
					break;
				}

				//패킷 도착 후 처리
				switch (header->ID) {
				case MOVE:
				{

					UpdateState* updatePkt = (UpdateState*)buf;
					for (int i = 0; i < updatePkt->numPlayers; ++i)
					{
						PlayerState pState;

						pState.Player_ID = updatePkt->players[i].Player_ID;
						pState.pos_x = updatePkt->players[i].pos_x;
						pState.pos_y = updatePkt->players[i].pos_y;
						pState.pos_z = updatePkt->players[i].pos_z;
						pState.Lookx = updatePkt->players[i].Lookx;
						pState.Looky = updatePkt->players[i].Looky;
						pState.Lookz = updatePkt->players[i].Lookz;
						pState.fire = updatePkt->players[i].fire;
						gGameFramework.recv_Queue.push(pState);
					}
					break;
				}
				}

				if (recvByte - header->size > 0)
				{
					memmove(buf, buf + header->size, recvByte - (header->size));
				}
				recvByte -= header->size;
			}
		}

		//리팩토링 - 홍성호
		MovePacket packet;
		while (gGameFramework.send_Queue.pop(packet))
		{
			retval = send(sock, (char*)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		} 
	}
	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}


// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char cbuf[BUFSIZE * 2];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
}

// 소켓 함수 오류 출력
void DisplayError(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s\r\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
void DisplayError_Quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s\r\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(1);
}
