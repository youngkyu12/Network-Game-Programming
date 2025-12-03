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
	

	CPlayer* m_pPlayer = NULL;



#ifdef _WITH_DRAW_AXIS
	CGameObject* m_pWorldAxis = NULL;
#endif

public:
	CWallsObject** m_ppObjects = NULL;
	CWallsObject* m_pWallsObject = NULL;
	void BuildObjects();
	void ReleaseObjects();
	void Animate(float fElapsedTime);
	void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);


};
