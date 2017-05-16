#include "GameClass.h"



GameClass::GameClass() :
	pDevice(nullptr),
	pSwapChain(nullptr),
	pCommandQueue(nullptr),
	pRTVDescHeap(nullptr),
	pCommandList(nullptr),
	frameIndex(0),
	rtvDescSize(0) {
	for (int i = 0; i < frameBufferCount; i++) {
		ZeroMemory(&(renderTargets[i]), sizeof(ID3D12Resource));
		ZeroMemory(&(commandAllocators[i]), sizeof(ID3D12CommandAllocator));
		ZeroMemory(&(fences[i]), sizeof(ID3D12Fence));
		fenceValue[i] = 0;
	}
}

bool GameClass::InitD3D() {
	HRESULT hr = S_OK;
	// Create a dxgi factory to create Direct3D Device
	IDXGIFactory4* dxgiFactory = nullptr;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&dxgiFactory);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed creating dxgi factory", L"3D-Tetris");
		return false;
	}

	// Pointer to adapter (graphics card interface)
	IDXGIAdapter1* pAdapter;
	// Flag to set valid adapter
	bool adapterFound = false;

	// Start searching for compatible adapters
	for (int adapterIndex = 0; dxgiFactory->EnumAdapters1(adapterIndex, &pAdapter); adapterIndex++) {
		// Get adapters description
		DXGI_ADAPTER_DESC1 adapterDesc;
		pAdapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// skip adapter since its software rendered
			continue;
		}
		// Try creating device adapter
		hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr)) {
			adapterFound = true;
			break;
		}
	}

	if (!adapterFound) {
		ERRORBOX(L"Current machine does not support direct3D12", L"3D-Tetris");
		return false;
	}

	// Create the device
	hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&pDevice);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to create the device", L"3D-Tetris");
		return false;
	}

	// Create the render target view command view
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // Default flags
	commandQueueDesc.NodeMask = 0; // Select primary adapter (values other than zero only for multiadapters
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // Normal priority
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // Default command list type (accepts all commands)

	hr = pDevice->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&pCommandQueue);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to create command queue", L"3D-Tetris");
		return false;
	}

	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferCount = frameBufferCount;
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = windowWidth;
	rc.bottom = windowHeight;
	AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, NULL, NULL);
	scd.BufferDesc.Width = rc.right - rc.left;
	scd.BufferDesc.Height = rc.bottom - rc.top;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.OutputWindow = hWnd;
	scd.Windowed = true;
	
	IDXGISwapChain* pTempSwapChain = nullptr;
	hr = dxgiFactory->CreateSwapChain(pDevice, &scd, &pTempSwapChain);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed creating swap chain", L"3D-Tetris");
		return false;
	}

	pSwapChain = (IDXGISwapChain3*)pTempSwapChain;

	frameIndex = pSwapChain->GetCurrentBackBufferIndex();

	// Create the render target descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dhd;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // Render target view descriptor heap
	dhd.NodeMask = 0; // default hardward index (not zero for multiple hardware interfaces)
	dhd.NumDescriptors = frameBufferCount;
	dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // Not shader visible

	hr = pDevice->CreateDescriptorHeap(&dhd, __uuidof(ID3D12DescriptorHeap), (void**)&pRTVDescHeap);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed creating render target view descriptor heap", L"3D-Tetris");
		return false;
	}

	rtvDescSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Grap handle to first descriptor in descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle.ptr = pRTVDescHeap->GetCPUDescriptorHandleForHeapStart().ptr;

	for (int i = 0; i < frameBufferCount; i++) {
		hr = pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&(renderTargets[i]));
		if (FAILED(hr)) {
			ERRORBOX(L"Failed to get swap chain buffer", L"3D-Tetris");
			return false;
		}

		pDevice->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);

		rtvHandle.ptr += rtvDescSize;
	}

	// Create the command allocators
	for (int i = 0; i < frameBufferCount; i++) {
		hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&commandAllocators[i]);
		if (FAILED(hr)) {
			ERRORBOX(L"Failed to create command allocator", L"3D-Tetris");
			return false;
		}
	}

	// Create command list
	hr = pDevice->CreateCommandList(0, // Primary adapter
		D3D12_COMMAND_LIST_TYPE_DIRECT, // Direct list
		commandAllocators[0], // Start of command allocator array
		NULL, // DO not bind to pipeline stage
		__uuidof(ID3D12GraphicsCommandList), 
		(void**)&pCommandList);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to create command list", L"3D-Tetris");
		return false;
	}
	// Close command list since created in recording state
	pCommandList->Close();

	// Create fences
	for (int i = 0; i < frameBufferCount; i++) {
		fenceValue[i] = 0;
		hr = pDevice->CreateFence(fenceValue[0], D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fences[i]);
		if (FAILED(hr)) {
			ERRORBOX(L"Failed to create fence", L"3D-Tetris");
			return false;
		}
	}

	// Create fence event
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent) {
		ERRORBOX(L"Failed to create fence event", L"3D-Tetris");
		return false;
	}

	// Return success if everything went well
	return true;
}

void GameClass::Update() {
	// Update game logic
}

bool GameClass::UpdatePipeline() {
	HRESULT hr = S_OK;

	// Wait for GPU to finish using command allocator
	WaitForPreviousFrame();

	// Reset allocators to free memory the command list was stored in
	hr = commandAllocators[frameIndex]->Reset();
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to release command allocator", L"3D-Tetris");
		return false;
	}

	// Reset command list to start recording
	hr = pCommandList->Reset(commandAllocators[frameIndex], NULL);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to reset command list", L"3D-Tetris");
		return false;
	}

	// Transition resource barrier
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRTVDescHeap->GetCPUDescriptorHandleForHeapStart(), rtvDescSize);

	// Draw commands
	pCommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	const float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

	pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Transistion resource back
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Stop drawing
	hr = pCommandList->Close();
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to close command list", L"3D-Tetris");
		return false;
	}

	return true;
}

bool GameClass::Render() {
	HRESULT hr = S_OK;
	
	// Update the pipeline
	if (!UpdatePipeline()) {
		ERRORBOX(L"Failed to update pipeline", L"3D-Tetris");
		return false;
	}

	// Create an array of command lists
	ID3D12CommandList* pCommandLists[] = { pCommandList };

	// Execute array of command lists
	pCommandQueue->ExecuteCommandLists(1, pCommandLists);

	hr = pCommandQueue->Signal(fences[frameIndex], fenceValue[frameIndex]);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to signal command queue", L"3D-Tetris");
		return false;
	}

	// Present the current backbuffer
	hr = pSwapChain->Present(0, NULL);
	if (FAILED(hr)) {
		ERRORBOX(L"Failed to present swap chain", L"3D-Tetris");
		return false;
	}

	return true;
}

void GameClass::Cleanup() {
	// Wait for the gpu to finish with all frames
	for (int i = 0; i < frameBufferCount; i++) {
		frameIndex = i;
		WaitForPreviousFrame();
	}

	// Remove swap chain from being full screen
	BOOL fs = false;
	if (pSwapChain->GetFullscreenState(&fs, NULL))
		pSwapChain->SetFullscreenState(false, NULL);

	util::SafeRelease(pDevice);
	util::SafeRelease(pSwapChain);
	util::SafeRelease(pCommandQueue);
	util::SafeRelease(pRTVDescHeap);
	util::SafeRelease(pCommandList);

	for (int i = 0; i < frameBufferCount; i++) {
		util::SafeRelease(renderTargets[i]);
		util::SafeRelease(commandAllocators[i]);
		util::SafeRelease(fences[i]);
	}
}

bool GameClass::WaitForPreviousFrame() {
	HRESULT hr = S_OK;
	frameIndex = pSwapChain->GetCurrentBackBufferIndex();
	if (fences[frameIndex]->GetCompletedValue() < fenceValue[frameIndex]) {
		hr = fences[frameIndex]->SetEventOnCompletion(fenceValue[frameIndex], fenceEvent);
		if (FAILED(hr)) {
			ERRORBOX(L"Failed to set event on completion", L"3D-Tetris");
			return false;
		}

		WaitForSingleObject(fenceEvent, INFINITE);
	}

	fenceValue[frameIndex++];
}

GameClass::~GameClass() {
}
