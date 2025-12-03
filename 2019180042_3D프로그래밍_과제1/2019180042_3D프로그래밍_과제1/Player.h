#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "SocketQueue.h"

class CScene;


class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	bool PrevFire = false;// 이전 발사 상태 체크용
	bool PrevShield = false;// 이전 쉴드 상태 체크용
	CShieldObject* m_pShieldObject = nullptr;

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	int hp = DEAD_PLAYER;

	CCamera* m_pCamera = NULL;

	static CPlayer** s_ppPlayers;
	static int       s_nPlayers;

public:
	void SetPosition(float x, float y, float z);

	void SetLook(XMFLOAT3& xmf3Look);

	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);

	void Move(XMFLOAT3& xmf3Shift);

	void MoveUpdate(PlayerState player, float fTimeElapsed = 0.016f);
	void Update(float fTimeElapsed = 0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	static CScene* s_pSceneRef;

	static void RegisterPlayers(CPlayer** ppPlayers, int nPlayers, CScene* pScene) {
		s_ppPlayers = ppPlayers;
		s_nPlayers = nPlayers;
		s_pSceneRef = pScene;
	}


	virtual void FireBullet() {}

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }
};

#define BULLETS					50

class CTankPlayer : public CPlayer
{
public:
	CTankPlayer();
	virtual ~CTankPlayer();

	float			m_fBulletEffectiveRange = 150.0f;
	CBulletObject*	m_ppBullets[BULLETS];


	// 충돌처리
	void FireBullet();

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};

