#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	CScene(CPlayer* pPlayer);
	virtual ~CScene();

private:
	int							m_nObjects = 0;
	CGameObject** m_ppObjects = NULL;

	CPlayer* m_pPlayer = NULL;



#ifdef _WITH_DRAW_AXIS
	CGameObject* m_pWorldAxis = NULL;
#endif

public:
	CWallsObject* m_pWallsObject = NULL;
	void BuildObjects();
	void ReleaseObjects();
	void Animate(float fElapsedTime);
	void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	void CheckPlayerByWallCollision();
	void CheckObjectByWallCollisions();
	void CheckObjectByObjectCollisions();
	void CheckObjectByBulletCollisions();

	float shieldtime = 0.0f;
	bool shield = false;
	bool SceneWin = false;
};
