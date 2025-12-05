//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "GraphicsPipeline.h" // CGraphicsPipeline 관련 오류(E0276) 해결
extern uint8_t MyPlayerID;

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();

	BuildObjects();
	ResetPlayerLists ();
	m_eGameState = EGameState::None;
	_tcscpy_s(m_pszFrameRate, _T("Project ("));
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);
	// GameObject에서 Player로 변환 과정
	//for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
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
	m_pCamera = new CCamera();
	m_pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	m_pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	m_pCamera->SetFOVAngle(60.0f);

	m_pCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	CTankMesh* pTankMesh = new CTankMesh(6.0f, 6.0f, 6.0f);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_pPlayer[i] = new CTankPlayer();
		m_pPlayer[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_pPlayer[i]->SetMesh(pTankMesh);
		if (i == 0) {
			m_pPlayer[i]->SetColor(RGB(255, 0, 0));
			m_pPlayer[i]->SetCamera(m_pCamera);
			m_pPlayer[i]->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
		}
		else {
			m_pPlayer[i]->SetColor(RGB(0, 0, 255));
		}
	}

	m_pScene = new CScene(m_pPlayer[0]);
	m_pScene->BuildObjects();
	CPlayer::RegisterPlayers(reinterpret_cast<CPlayer**>(m_pPlayer), MAX_PLAYERS, m_pScene);
	// 총알 생성
	/* GameObject에서 Player로 변환 과정
	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 4.0f, 1.0f);
	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(500.0f);
		m_ppBullets[i]->SetMesh(pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(120.0f);
		m_ppBullets[i]->SetActive(false);
	}
	*/
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pCamera) delete m_pCamera;

	// GameObject에서 Player로 변환 과정
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (m_pPlayer[i]) delete m_pPlayer[i];
	}
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_MOUSEMOVE:
		//::SetCapture ( hWnd );
		break;
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		if (stop) {
			stop = false;
		}
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
			SetRunning(false);
			::PostQuitMessage(0);
			break;
		case 'D':
			if (stop) {
				stop = false;
				::SetCapture(hWnd);
			}
			else {
				stop = true;
				::ReleaseCapture();
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
		/*
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		*/
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
	keyPKT.header.ID = UPDATE;
	keyPKT.header.size = sizeof(MovePacket);
	keyPKT.header.GameState = m_eGameState;

	bool keyinput = false;
	bool mouseinput = false;
	static bool A_PressedPrev = false; // 이전상태 기억
	static bool F_PressedPrev = false; // 이전상태 기억
	static bool P_PressedPrev = false; // 이전상태 기억
	
	static UCHAR pKeyBuffer[256];
	if ( GetKeyboardState ( pKeyBuffer ) )
	{
		keyPKT.keyW = (pKeyBuffer['W'] & 0xF0) ? 1 : 0;// char w 전송
		keyPKT.keyS = (pKeyBuffer['S'] & 0xF0) ? 1 : 0; // char s 전송

		if (keyPKT.keyS || keyPKT.keyW)
		{
			keyinput = true;
		}

		bool A_PressedNow = (pKeyBuffer['A'] & 0xF0) ? true : false;
		// A키 단발성으로 입력받기, 게임시작전엔 발사 금지
		if (A_PressedNow == true && A_PressedPrev == false && m_pPlayer[0]->PrevFire == false && m_eGameState == Playing)
		{
			keyPKT.FireFlag = 1;
			keyinput = true;
		}
		else
		{
			keyPKT.FireFlag = 0;
		}
		A_PressedPrev = A_PressedNow;

		bool F_PressedNow = (pKeyBuffer['F'] & 0xF0) ? true : false;
		if (F_PressedNow == true && F_PressedPrev == false && m_pPlayer[0]->m_pShieldObject->m_bActive == false && m_eGameState == Playing)
		{
			keyPKT.shield = 1;
			keyinput = true;
		}
		else
		{
			keyPKT.shield = 0;
		}
		F_PressedPrev = F_PressedNow;

		bool P_PressedNow = (pKeyBuffer['P'] & 0xF0) ? true : false;
		if (P_PressedNow == true && P_PressedPrev == false && m_eGameState == None)
		{
			m_eGameState = Ready;
			keyPKT.header.GameState = m_eGameState;
			keyinput = true;
		}
		P_PressedPrev = P_PressedNow;
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

	if (m_pPlayer )
	if (keyinput || mouseinput)
	{
		send_Queue.push(keyPKT);
	}
	
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	//GameObject에서 Player로 변환 과정
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if ( m_pPlayer[i]->hp != DEAD_PLAYER )m_pPlayer[i]->Animate ( fTimeElapsed );
	}
	//
	if (m_pScene) m_pScene->Animate(fTimeElapsed);
	
	/* GameObject에서 Player로 변환 과정
	for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i] && m_ppBullets[i]->m_bActive)
		{
			m_ppBullets[i]->Animate(fTimeElapsed);
		}
	}
	*/
}

void CGameFramework::FrameAdvance()
{ 
	m_GameTimer.Tick(60.0f);

	if ( !IsGameOver () ) {
		ProcessInput ();
		HandlePacket ();

		//GameObject에서 Player로 변환 과정 카메라 업데이트만 실행
		m_pPlayer[0]->Update ( m_GameTimer.GetTimeElapsed () );
		//m_pPlayer->Update(m_GameTimer.GetTimeElapsed());

		AnimateObjects ();
	}

	if ( IsGameOver () ) {
		RenderResultScene ( m_hDCFrameBuffer );
	}
	else {
		ClearFrameBuffer ( RGB ( 255 , 255 , 255 ) );
		RenderScene();
		DrawUI();
	}
	/*GameObject에서 Player로 변환 과정
	for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i] && m_ppBullets[i]->m_bActive)
		{
			m_ppBullets[i]->Render(m_hDCFrameBuffer, m_pCamera);
		}
	}
	*/
	
	PresentFrameBuffer();

	m_fExplosionElapsedTime += fDeltaTime;

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::RenderScene()
{
	// 공통 셋업
	CGraphicsPipeline::SetViewport(&m_pCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&m_pCamera->m_xmf4x4ViewPerspectiveProject);

	// 1) 씬과 플레이어를 하나의 리스트로 결합
	std::vector<CGameObject*> drawList;
	drawList.reserve(static_cast<size_t>((m_pScene ? m_pScene->m_nObjects : 0)) + static_cast<size_t>(MAX_PLAYERS)); // lnt-arithmetic-overflow 경고 해결

	if (m_pScene)
	{
		// 벽은 먼저 그리기(배경)
		m_pScene->m_pWallsObject->Render(m_hDCFrameBuffer, m_pCamera);

		// 씬 오브젝트 추가
		for (int i = 0; i < (m_pScene ? m_pScene->m_nObjects : 0); ++i)
		{
			if (m_pScene->m_ppObjects[i] && m_pScene->m_ppObjects[i]->m_bActive)
				drawList.push_back(m_pScene->m_ppObjects[i]);
		}
	}

	// 플레이어 추가
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (m_pPlayer[i] && m_pPlayer[i]->hp > 0)
			drawList.push_back(m_pPlayer[i]);
	}

	// 2) 뷰 행렬로 변환한 Z(원거리 먼저) 기준 정렬
	XMMATRIX V = XMLoadFloat4x4(&m_pCamera->m_xmf4x4View);
	std::sort(drawList.begin(), drawList.end(),
		[&](CGameObject* a, CGameObject* b)
		{
			XMVECTOR va = XMVector3TransformCoord(XMLoadFloat3(&a->GetPosition()), V);
			XMVECTOR vb = XMVector3TransformCoord(XMLoadFloat3(&b->GetPosition()), V);
			float za = XMVectorGetZ(va);
			float zb = XMVectorGetZ(vb);
			return za > zb; // 원거리 먼저
		});

	// 3) 정렬된 순서로 면 채움 후 와이어 렌더
	for (CGameObject* obj : drawList)
	{
		// 필요 시 반투명 alpha로 채우기: RenderFilled(hdc, pCamera, color, alpha)
		//obj->RenderFilled(m_hDCFrameBuffer, m_pCamera, RGB(128, 128, 128));
		obj->Render(m_hDCFrameBuffer, m_pCamera);
	}
}

void CGameFramework::HandlePacket()
{
	// 리팩토링
	PlayerState player;
	while (recv_Queue.pop(player)) 
	{
		XMFLOAT3 Look = { player.Lookx,player.Looky,player.Lookz };
		if ( player.Player_ID == MyPlayerID )
		{	//GameObject에서 Player로 변환 과정 m_pPlayer -> m_pPlayer[0]
			int oldHp = m_pPlayer[0]->hp;
			m_pPlayer[0]->hp = player.hp;
			m_pPlayer[0]->SetPosition ( player.pos_x , player.pos_y , player.pos_z );
			m_pPlayer[0]->SetLook ( Look );
			if ( player.fire == 1 && m_pPlayer[0]->PrevFire == false )
			{
				m_pPlayer[0]->FireBullet ();
			}
			m_pPlayer[0]->PrevFire = player.fire; // 현재 상태를 과거의 상태값으로 저장.(다음 턴 사용을 위해서)
			m_pPlayer[0]->m_pShieldObject->m_bActive = player.shield;

			if ( oldHp != DEAD_PLAYER && player.hp == DEAD_PLAYER ) OnPlayerDeath ( MyPlayerID );
			else if ( oldHp == DEAD_PLAYER && player.hp != DEAD_PLAYER ) OnPlayerSpawn ( MyPlayerID );
		}
		else
		{
			int objIndex = -1;
			// 내 ID보다 작으면 그대로... 크면 -1하여 땡겨줌(원래 내가 차지했어야할 공간을 땡겨주기)
			if ( player.Player_ID < MyPlayerID )
			{
				objIndex = player.Player_ID + 1;
			}
			else
			{
				objIndex = player.Player_ID;
			}

			if ( m_pScene && objIndex >= 0 && objIndex < MAX_PLAYERS ) //10명까지
			{
				//GameObject에서 Player로 변환 과정 m_pPlayer[objIndex]
				CPlayer* Other_player = m_pPlayer[objIndex];
				if ( Other_player )
				{
					int oldHp = Other_player->hp;
					Other_player->hp = player.hp;
					Other_player->SetPosition ( player.pos_x , player.pos_y , player.pos_z );
					Other_player->SetLook ( Look );
					if ( player.fire == 1 && Other_player->PrevFire == false )
					{
						Other_player->FireBullet ();
					}
					Other_player->PrevFire = player.fire;
					if (Other_player->m_pShieldObject) {
						Other_player->m_pShieldObject->m_bActive = player.shield;
					}

					if ( oldHp != DEAD_PLAYER && player.hp == DEAD_PLAYER ) OnPlayerDeath ( player.Player_ID );
					else if ( oldHp == DEAD_PLAYER && player.hp != DEAD_PLAYER ) OnPlayerSpawn ( player.Player_ID );

				}

			}
		}
		if ( m_eGameState == Playing && m_alivePlayers.size () <= 1 ) 
		{
			TriggerGameOver ( "모든 플레이어가 사망했거나 최후의 생존자만 남았습니다." );
		}
	}
}

void CGameFramework::DrawUI()
{
	int oldBkMode = ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
	COLORREF oldTextColor = ::SetTextColor(m_hDCFrameBuffer, RGB(0, 0, 255));
	const TCHAR* bannerText = NULL;

	// Playing 상태 경과 시간 누적용(함수 정적 변수)
	static float s_playingElapsedSec = 0.0f;
	if (m_eGameState == Playing) {
		// 프레임 경과 시간 누적
		s_playingElapsedSec += m_GameTimer.GetTimeElapsed();
	}
	else {
		// 다른 상태로 전환 시 리셋
		s_playingElapsedSec = 0.0f;
	}

	// 1) 상단 중앙 배너 그리기
	switch (m_eGameState)
	{
	case None:
		bannerText = _T("\"P\"를 눌러 준비하세요");
		break;
	case Ready:
		bannerText = _T("준비완료");
		break;
	case Playing:
		if (s_playingElapsedSec < 3.0f) {
			bannerText = _T("게임시작!");
		}
		else {
			bannerText = _T("");; // 3초 이후에는 배너 미표시
		}
		break;
	}

	const int bannerMarginTop = 8;    // 상단 여백
	const int bannerHeightPx = 36;    // 폰트 높이 (픽셀). 크게/작게 조절
	const int bannerWeight = FW_BOLD; // 굵기(FW_NORMAL, FW_BOLD 등)

	HFONT hBannerFont = ::CreateFont(
		bannerHeightPx,        // nHeight
		0,                     // nWidth (0: 높이에 맞춰 자동)
		0,                     // nEscapement
		0,                     // nOrientation
		bannerWeight,          // fnWeight
		FALSE,                 // fdwItalic
		FALSE,                 // fdwUnderline
		FALSE,                 // fdwStrikeOut
		DEFAULT_CHARSET,       // fdwCharSet
		OUT_DEFAULT_PRECIS,    // fdwOutputPrecision
		CLIP_DEFAULT_PRECIS,   // fdwClipPrecision
		ANTIALIASED_QUALITY,   // fdwQuality (가독성 향상)
		DEFAULT_PITCH | FF_DONTCARE, // fdwPitchAndFamily
		_T("Segoe UI")         // lpszFace (원하는 폰트명)
	);

	HFONT hOldFont = (HFONT)::SelectObject(m_hDCFrameBuffer, hBannerFont);
	::SetTextColor(m_hDCFrameBuffer, RGB(20, 20, 20)); // 배너 색상

	SIZE bannerSize{};
	::GetTextExtentPoint32(m_hDCFrameBuffer, bannerText, (int)_tcslen(bannerText), &bannerSize);

	int clientW = m_rcClient.right - m_rcClient.left;
	int bannerX = m_rcClient.left + (clientW - bannerSize.cx) / 2; // 중앙 정렬 X
	int bannerY = bannerMarginTop;                                 // 상단 여백 Y

	::TextOut(m_hDCFrameBuffer, bannerX, bannerY, bannerText, (int)_tcslen(bannerText));

	// 폰트/색상 복원 및 삭제
	::SelectObject(m_hDCFrameBuffer, hOldFont);
	::DeleteObject(hBannerFont);

	// 2) 플레이어 상태 리스트(기존 UI)
	::SetTextColor(m_hDCFrameBuffer, RGB(0, 0, 255));

	int x = m_rcClient.left + 10;
	int y = 20;
	int lineHeight = 20;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		// 살아있는 플레이어만 표시
		if (m_pPlayer[i] && m_pPlayer[i]->hp != DEAD_PLAYER)
		{
			TCHAR szInfo[64];

			// 내 캐릭터는 빨간색
			if (i == MyPlayerID) {
				::SetTextColor(m_hDCFrameBuffer, RGB(255, 0, 0));
				_stprintf_s(szInfo, _T("P %d (Me) HP:%d"), i, m_pPlayer[i]->hp);
			}
			else {
				::SetTextColor(m_hDCFrameBuffer, RGB(0, 255, 0)); 
				_stprintf_s(szInfo, _T("P %d HP:%d"), i, m_pPlayer[i]->hp);
			}

			::TextOut(m_hDCFrameBuffer, x, y, szInfo, _tcslen(szInfo));
			y += lineHeight; // 다음 줄로 이동
		}
	}
	::SetBkMode(m_hDCFrameBuffer, oldBkMode);
	::SetTextColor(m_hDCFrameBuffer, oldTextColor);
}

void CGameFramework::SetMyState(uint8_t newState)
{
	m_eGameState = newState;
}

uint8_t CGameFramework::GetMyState()
{
	return m_eGameState;
}

void CGameFramework::ResetPlayerLists () {
	m_alivePlayers.clear ();
	m_deadPlayers.clear ();
	m_vFinalRanks.clear ();
	m_szGameOverReason[0] = 0;
}

void CGameFramework::OnPlayerSpawn ( uint8_t id ) {
	// 살아있는 집합에 추가
	m_alivePlayers.insert ( id );
	// 죽은 목록에 존재하면 제거(리스폰 대비)
	auto it = std::find ( m_deadPlayers.begin () , m_deadPlayers.end () , id );
	if ( it != m_deadPlayers.end () ) m_deadPlayers.erase ( it );
}

void CGameFramework::OnPlayerDeath ( uint8_t id ) {
	// 살아있는 집합에서 제거
	m_alivePlayers.erase ( id );
	// 중복 방지 후 앞에 삽입(방금 죽은 사람이 앞쪽)
	if ( !IsInDead ( id ) ) m_deadPlayers.push_front ( id );
}

void CGameFramework::TriggerGameOver ( const char* reason ) {
	if ( IsGameOver () ) return;
	m_eGameState = GameOver;

	// 최종 랭킹 구성: 생존자(우승) → 죽은 순서(앞에서부터 순위)
	m_vFinalRanks.clear ();
	for ( auto id : m_alivePlayers ) m_vFinalRanks.push_back ( id ); // 여러 명 생존 시 동순위 취급
	for ( auto id : m_deadPlayers )  m_vFinalRanks.push_back ( id );

	if ( reason ) {
#ifdef UNICODE
		size_t outLen = 0;
		mbstowcs_s ( &outLen , m_szGameOverReason , reason , _TRUNCATE );
#else
		strncpy_s ( m_szGameOverReason , reason , _TRUNCATE );
#endif
	}
}

void CGameFramework::RenderResultScene ( HDC hdc ) {
	// 배경 클리어
	ClearFrameBuffer ( RGB ( 240 , 240 , 240 ) );

	// 텍스트 설정
	::SetBkMode ( m_hDCFrameBuffer , TRANSPARENT );
	::SetTextColor ( m_hDCFrameBuffer , RGB ( 20 , 20 , 20 ) );

	int x = 40;
	int y = 40;
	const int dy = 24;

	// 제목
	TextOut ( m_hDCFrameBuffer , x , y , _T ( "Game Over" ) , ( int )_tcslen ( _T ( "Game Over" ) ) );
	y += dy;

	// 랭킹 출력
	y += dy;
	TextOut ( m_hDCFrameBuffer , x , y , _T ( "Ranking:" ) , ( int )_tcslen ( _T ( "Ranking:" ) ) );
	y += dy;

	for ( size_t i = 0; i < m_vFinalRanks.size (); ++i ) {
		TCHAR line[64];
		_stprintf_s ( line , _T ( "%zu위: Player %u" ) , i + 1 , ( unsigned )m_vFinalRanks[i] );
		TextOut ( m_hDCFrameBuffer , x , y , line , ( int )_tcslen ( line ) );
		y += dy;
	}

	// 안내
	y += dy;
	TextOut ( m_hDCFrameBuffer , x , y , _T ( "ESC: 종료 " ) , 8 );
}


/*
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
*/