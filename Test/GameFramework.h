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
	// �����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 

	void OnDestroy();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	// ���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.     

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	// ���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�. 

	void BuildObjects();
	void ReleaseObjects();
	// �������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
    // �����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	// CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�. 

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 

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