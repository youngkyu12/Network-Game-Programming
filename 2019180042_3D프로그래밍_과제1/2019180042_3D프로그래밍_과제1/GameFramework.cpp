//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();

	BuildObjects();

	_tcscpy_s(m_pszFrameRate, _T("Project ("));
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);
	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CGameFramework::BuildFrameBuffer()
{
	::GetClientRect(m_hWnd, &m_rcClient);

	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::BuildObjects()
{
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	CTankMesh* pTankMesh = new CTankMesh(6.0f, 6.0f, 6.0f);
	m_pPlayer = new CTankPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pTankMesh);
	m_pPlayer->SetColor(RGB(255, 0, 0));
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));

	m_pScene = new CScene(m_pPlayer);
	m_pScene->BuildObjects();

	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 4.0f, 1.0f);
	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(120.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_MOUSEMOVE:
		::SetCapture ( hWnd );
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 'A':
			FirePacket firePkt;
			firePkt.header.ID = FIRE;
			firePkt.header.size = sizeof(FirePacket);
			firePkt.FireFlag = true;
			//send(sock, (char*)&firePkt, firePkt.header.size, 0);
			break;
		case 'D':
			if (stop) {
				stop = false;
			}
			else {
				stop = true;
			}
			break;
		}
		break;
	default:
		break;
	}
}

// 키보드 메시지 처리
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	MovePacket keyPKT;
	keyPKT.header.ID = MOVE;
	keyPKT.header.size = sizeof(MovePacket);

	bool keyinput = false;
	bool mouseinput = false;

	static UCHAR pKeyBuffer[256];
	if ( GetKeyboardState ( pKeyBuffer ) )
	{
		keyPKT.keyW = (pKeyBuffer['W'] & 0xF0) ? 1 : 0;// char w 전송
		keyPKT.keyS = (pKeyBuffer['S'] & 0xF0) ? 1 : 0; // char s 전송

		if (keyPKT.keyS || keyPKT.keyW) {
			keyinput = true;
		}
	}

	if ( !stop ) {
		if ( GetCapture () == m_hWnd )
		{
			SetCursor ( NULL );
			POINT ptCursorPos;
			GetCursorPos ( &ptCursorPos );
			keyPKT.yaw = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			SetCursorPos ( m_ptOldCursorPos.x , m_ptOldCursorPos.y );

			if (keyPKT.yaw != 0) {
				mouseinput = true;
			}
		}
	}

	// [11/20]
	// 네트워크 스레드가 있는데 렌더하는 주 스레드에서 Send가 발생하면 프레임이 많이 떨어져서 끊기는 현상이 자주 발생합니다.
	// 그래서 이렇게 안 하고 Send_Queue에 push해서 사용하도록 변경하겠습니다.
	
	if (keyinput || mouseinput)
	{
		send_Queue.push(keyPKT);
	}
	
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed);
	if (m_pScene) m_pScene->Animate(fTimeElapsed);
	for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fTimeElapsed);
	}
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(60.0f);
	ProcessInput();

	HandlePacket();
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());

	AnimateObjects();

	ClearFrameBuffer(RGB(255, 255, 255));

	CCamera* pCamera = m_pPlayer->GetCamera();
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera);

	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(m_hDCFrameBuffer, pCamera);
	
	PresentFrameBuffer();

	m_fExplosionElapsedTime += fDeltaTime;

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::HandlePacket()
{
	// 리팩토링 - 홍성호
	PlayerState player;
	while (recv_Queue.pop(player)) 
	{
		XMFLOAT3 Look = { player.Lookx,player.Looky,player.Lookz};
		if (player.Player_ID == 0)
		{
			m_pPlayer->SetPosition(player.pos_x, player.pos_y, player.pos_z);
			m_pPlayer->SetLook(Look);
			if (player.fire == true) {
				FireBullet(m_pPlayer->GetPosition(), m_pPlayer->GetUp(), m_pPlayer->m_xmf4x4World);
			}
		}
		else if (player.Player_ID == 1)
		{
			if (m_pScene && m_pScene->m_ppObjects[0])
			{
				m_pScene->m_ppObjects[0]->SetPosition(player.pos_x, player.pos_y, player.pos_z);
				m_pScene->m_ppObjects[0]->LookTo(Look, Up);
				m_pScene->m_ppObjects[0]->Rotate(90.0f, 0.0f, 0.0f);
				if (player.fire == true) {
					FireBullet(m_pScene->m_ppObjects[0]->GetPosition(), m_pScene->m_ppObjects[0]->GetUp(), m_pScene->m_ppObjects[0]->m_xmf4x4World);
				}
			}
		}
	}
}


void CGameFramework::FireBullet(XMFLOAT3 pos, XMFLOAT3 Up, XMFLOAT4X4 m_xmf4x4World)
{

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject)
	{
		XMFLOAT3 xmf3Position = pos;
		XMFLOAT3 xmf3Direction = Up;
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetColor(RGB(255, 0, 0));
		pBulletObject->SetActive(true);
	}
}
