#include "stdafx.h"
#include "GraphicsPipeline.h"

XMFLOAT4X4* CGraphicsPipeline::m_pxmf4x4World = NULL;
XMFLOAT4X4* CGraphicsPipeline::m_pxmf4x4ViewProject = NULL;
CViewport* CGraphicsPipeline::m_pViewport = NULL;

void CGraphicsPipeline::SetViewPerspectiveProjectTransform(XMFLOAT4X4* pxmf4x4ViewPerspectiveProject)
{
	m_pxmf4x4ViewProject = pxmf4x4ViewPerspectiveProject;
}

void CGraphicsPipeline::SetViewOrthographicProjectTransform(XMFLOAT4X4* pxmf4x4OrthographicProject)
{
	m_pxmf4x4ViewProject = pxmf4x4OrthographicProject;
}

XMFLOAT3 CGraphicsPipeline::Transform(XMFLOAT3& xmf3Model)
{
	XMFLOAT3 xmf3Project = Project(xmf3Model);
	XMFLOAT3 f3Screen = ScreenTransform(xmf3Project);

	return(f3Screen);
}

XMFLOAT3 CGraphicsPipeline::Project(XMFLOAT3& xmf3Model)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Multiply(*m_pxmf4x4World, *m_pxmf4x4ViewProject);
	XMFLOAT3 xmf3Project = Vector3::TransformCoord(xmf3Model, xmf4x4Transform);

	return(xmf3Project);
}

XMFLOAT3 CGraphicsPipeline::ScreenTransform(XMFLOAT3& xmf3Project)
{
	XMFLOAT3 f3Screen = xmf3Project;

	float fHalfWidth = m_pViewport->m_nWidth * 0.5f;
	float fHalfHeight = m_pViewport->m_nHeight * 0.5f;
	f3Screen.x = m_pViewport->m_nLeft + (xmf3Project.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = m_pViewport->m_nTop + (-xmf3Project.y * fHalfHeight) + fHalfHeight;

	return(f3Screen);
}

// ===== 여기부터 추가 구현 =====
bool CGraphicsPipeline::ProjectToClip ( XMFLOAT3& xmf3Model , XMFLOAT4& xmf4Clip )
{
	// clip = [x y z w] = [model 1] * (World * ViewProj)
	XMMATRIX m = XMLoadFloat4x4 ( m_pxmf4x4World ) * XMLoadFloat4x4 ( m_pxmf4x4ViewProject );
	XMVECTOR pos = XMVectorSet ( xmf3Model.x , xmf3Model.y , xmf3Model.z , 1.0f );
	XMVECTOR clip = XMVector4Transform ( pos , m );
	XMStoreFloat4 ( &xmf4Clip , clip );
	return true;
}

static inline XMFLOAT4 LerpFloat4 ( const XMFLOAT4& a , const XMFLOAT4& b , float t )
{
	return XMFLOAT4 (
		a.x + ( b.x - a.x ) * t ,
		a.y + ( b.y - a.y ) * t ,
		a.z + ( b.z - a.z ) * t ,
		a.w + ( b.w - a.w ) * t
	);
}

bool CGraphicsPipeline::ClipAndProjectLine ( XMFLOAT3& p0 , XMFLOAT3& p1 , XMFLOAT3& ndc0 , XMFLOAT3& ndc1 )
{
	XMFLOAT4 c0 , c1;
	ProjectToClip ( p0 , c0 );
	ProjectToClip ( p1 , c1 );

	// 둘 다 근평면 뒤(z<0)면 버림
	if ( ( c0.z < 0.0f ) && ( c1.z < 0.0f ) ) return false;

	// 하나만 z<0이면 z=0(근평면)에서 교점으로 치환
	// t = z0 / (z0 - z1), c(t) = c0 + t*(c1 - c0)
	if ( c0.z < 0.0f )
	{
		float t = c0.z / ( c0.z - c1.z );
		c0 = LerpFloat4 ( c0 , c1 , t );
		c0.z = 0.0f; // 수치 안정
	}
	else if ( c1.z < 0.0f )
	{
		float t = c1.z / ( c1.z - c0.z );
		c1 = LerpFloat4 ( c1 , c0 , t );
		c1.z = 0.0f;
	}

	// 원근 분할(NDC)
	if ( fabsf ( c0.w ) < EPSILON || fabsf ( c1.w ) < EPSILON ) return false;

	XMFLOAT3 n0 ( c0.x / c0.w , c0.y / c0.w , c0.z / c0.w );
	XMFLOAT3 n1 ( c1.x / c1.w , c1.y / c1.w , c1.z / c1.w );

	ndc0 = n0;
	ndc1 = n1;
	return true;
}
