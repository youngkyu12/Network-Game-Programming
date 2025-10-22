#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Player.h"

class CGameFramework {
public:
	CGameFramework();
	~CGameFramework();

public:
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// 프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 

	void OnDestroy();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	// 스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.     

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	// 렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다. 

	void BuildObjects();
	void ReleaseObjects();
	// 렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다. 
    // 프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	// CPU와 GPU를 동기화하는 함수이다. 

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 

	void ChangeSwapChainState();
	void OnResizeBackBuffers();

	void MoveToNextFrame();

private:
	HINSTANCE 		m_hInstance = NULL;
	HWND 			m_hWnd = NULL;
	int 			m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	int 			m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	IDXGIFactory4*	m_pdxgiFactory = nullptr;

	IDXGISwapChain3*	m_pdxgiSwapChain = nullptr;

	ID3D12Device*		m_pd3dDevice = nullptr;

	bool 				m_bMsaa4xEnable = false;
	UINT 				m_nMsaa4xQualityLevels = 0;

	static const UINT 		m_nSwapChainBuffers = 2;

	UINT 			m_nSwapChainBufferIndex = 0;

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = nullptr;
	UINT 			m_nRtvDescriptorIncrementSize = 0;

	ID3D12Resource* m_pd3dDepthStencilBuffer = nullptr;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = nullptr;
	UINT 			m_nDsvDescriptorIncrementSize = 0;

	ID3D12CommandQueue* m_pd3dCommandQueue = nullptr;
	ID3D12CommandAllocator* m_pd3dCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* m_pd3dCommandList = nullptr;

	ID3D12Fence* m_pd3dFence = nullptr;
	UINT64 			m_nFenceValues[m_nSwapChainBuffers];
	HANDLE 			m_hFenceEvent = NULL;

	CScene* m_pScene = nullptr;
	CCamera* m_pCamera = nullptr;
	CPlayer* m_pPlayer = nullptr;

	POINT 			m_ptOldCursorPos{ 0, 0 };

	CGameTimer		m_GameTimer;

	_TCHAR m_pszFrameRate[50]{ 0 };
};