#pragma once

#include "Player.h"
#include "Scene.h"
#include "Timer.h"
#include "SocketQueue.h"
#include <unordered_set>
#include <deque>
#include <algorithm>

enum EGameState
{
	None = 0,
	Ready = 1,
	Playing = 2,
	GameOver = 3,
};

class CGameFramework
{
public:
	CGameFramework() {}
	~CGameFramework() {}

private:
	HINSTANCE					m_hInstance = NULL;
	HWND						m_hWnd = NULL;
	
	float						m_fExplosionElapsedTime = 0.0f;
	float						fDeltaTime = 1.0f;
	bool						Bomb = false;

	bool						m_bActive = true;

	RECT						m_rcClient;

	HDC							m_hDCFrameBuffer = NULL;
	HBITMAP						m_hBitmapFrameBuffer = NULL;
	HBITMAP						m_hBitmapSelect = NULL;

	CCamera* m_pCamera = NULL;
	CPlayer* m_pPlayer[MAX_PLAYERS];
	CScene* m_pScene = NULL;

	// 플레이어 상태 컨테이너
	std::unordered_set<uint8_t> m_alivePlayers; // 살아있는 ID 집합
	std::deque<uint8_t>         m_deadPlayers;  // 앞: 방금 죽은 순

	bool IsInDead ( uint8_t id ) const {
		return std::find ( m_deadPlayers.begin () , m_deadPlayers.end () , id ) != m_deadPlayers.end ();
	}

	// 게임 상태
	uint8_t m_eGameState = None;

	// 최종 랭킹(엔드 씬용): 앞이 높은 순위(1위)
	std::vector<uint8_t>        m_vFinalRanks;
	TCHAR                       m_szGameOverReason[128] = _T ( "" );

	CGameObject* m_pPickObject = NULL;
	CGameObject* m_pLockedObject = NULL;

	CGameTimer					m_GameTimer;

	POINT						m_ptOldCursorPos = { 500,500 }; // 게임 진행 중 고정되어 있는 마우스 위치

	_TCHAR						m_pszFrameRate[50];

	bool stop = true;
	bool m_isRunning = false;

	XMFLOAT3 Up = { 0,1,0 };

	float						m_fBulletEffectiveRange = 150.0f;
	//CBulletObject* m_ppBullets[BULLETS];

public:
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void HandlePacket();
	void DrawUI(); // hp 그리는 함수
	void SetMyState(uint8_t newState);
	uint8_t GetMyState();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetActive(bool bActive) { m_bActive = bActive; }

	// void FireBullet(XMFLOAT3 pos, XMFLOAT3 Up, XMFLOAT4X4 m_xmf4x4World);
	void SetRunning(bool running) { m_isRunning = running; }
	bool IsRunning() const { return m_isRunning; }

	// 상태 전이 처리 API
	void OnPlayerSpawn ( uint8_t id );
	void OnPlayerDeath ( uint8_t id );

	// 랭킹(죽은 순서) 조회: 앞에서부터 출력하면 됨
	std::deque<uint8_t> GetDeadOrder () const { return m_deadPlayers; }

	// 게임종료
	void TriggerGameOver ( const char* reason = nullptr ); // 게임 종료 전이
	bool IsGameOver () const { return m_eGameState == EGameState::GameOver; }

	// 초기화
	void ResetPlayerLists ();

protected:
	void RenderResultScene ( HDC hdc ); // 엔드 씬 렌더링

public:
	SendQueue send_Queue;
	RecvQueue recv_Queue;
};

