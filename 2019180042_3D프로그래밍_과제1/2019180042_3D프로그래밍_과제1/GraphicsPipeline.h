#pragma once

#include "GameObject.h"
#include "Camera.h"

class CGraphicsPipeline
{
private:
	static XMFLOAT4X4* m_pxmf4x4World;
	static XMFLOAT4X4* m_pxmf4x4ViewProject;
	static CViewport* m_pViewport;

public:
	static void SetWorldTransform(XMFLOAT4X4* pxmf4x4World) { m_pxmf4x4World = pxmf4x4World; }
	static void SetViewPerspectiveProjectTransform(XMFLOAT4X4* pxmf4x4ViewPerspectiveProject);
	static void SetViewOrthographicProjectTransform(XMFLOAT4X4* pxmf4x4OrthographicProject);
	static void SetViewport(CViewport* pViewport) { m_pViewport = pViewport; }

	static XMFLOAT3 ScreenTransform(XMFLOAT3& xmf3Project);
	static XMFLOAT3 Project(XMFLOAT3& xmf3Model);
	static XMFLOAT3 Transform(XMFLOAT3& xmf3Model);

	// 추가: 모델 좌표를 클립 공간(float4)으로 변환
	static bool ProjectToClip ( XMFLOAT3& xmf3Model , XMFLOAT4& xmf4Clip );

	// 추가: 근평면(z=0) 클리핑 후 NDC(x/w, y/w, z/w) 반환
	// 반환 false: 선분 전체가 근평면 뒤(두 점 모두 clip.z<0) → 스킵
	static bool ClipAndProjectLine ( XMFLOAT3& p0 , XMFLOAT3& p1 , XMFLOAT3& ndc0 , XMFLOAT3& ndc1 );

};
