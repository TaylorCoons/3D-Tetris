#pragma once
#include "WindowClass.h"
#include "Includes.h"

class GameClass :
	public WindowClass {
private:
	// Triple buffering
	static const int frameBufferCount = 3;
	// Direct 3d Device
	ID3D12Device* pDevice;
	// Swap chain
	IDXGISwapChain3* pSwapChain;
	// Command queue
	ID3D12CommandQueue* pCommandQueue;
	// Descriptor heap to hold render target view resources
	ID3D12DescriptorHeap* pRTVDescHeap;
	// Render target resources
	ID3D12Resource* renderTargets[frameBufferCount];
	// Command allocator
	ID3D12CommandAllocator* commandAllocators[frameBufferCount];
	// Command list
	ID3D12GraphicsCommandList* pCommandList;
	// Fences
	ID3D12Fence* fences[frameBufferCount];
	// Handle to fence event
	HANDLE fenceEvent;
	// Fence value
	UINT64 fenceValue[frameBufferCount];
	// Frame index
	int frameIndex;
	// Render target view descriptor size
	int rtvDescSize;


public:
	GameClass();
	// Initialize Direct3D
	bool InitD3D();
	// Update game logic
	void Update();
	// Update pipeline
	bool UpdatePipeline();
	// Render frame
	bool Render();
	// Cleanup Direct3D
	void Cleanup();
	// Wait for GPU to finish previous frame
	bool WaitForPreviousFrame();

	~GameClass();
};

