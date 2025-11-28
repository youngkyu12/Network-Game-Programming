#pragma once

#include "Player.h"
#include "Scene.h"
#include "Timer.h"
#include "SocketQueue.h"


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

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetActive(bool bActive) { m_bActive = bActive; }

	// void FireBullet(XMFLOAT3 pos, XMFLOAT3 Up, XMFLOAT4X4 m_xmf4x4World);
	void SetRunning(bool running) { m_isRunning = running; }
	bool IsRunning() const { return m_isRunning; }

public:
	SendQueue send_Queue;
	RecvQueue recv_Queue;
};

