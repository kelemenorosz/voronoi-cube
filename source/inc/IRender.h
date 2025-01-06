#pragma once

#include <d3d12.h>
#include <wrl.h>

//Abstract render class
class IRender {

public:

	virtual ~IRender() {};

	//Pure virtual functions
	virtual void LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription) = 0;
	virtual BOOL OnRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_CPU_DESCRIPTOR_HANDLE) = 0;
	virtual BOOL OnUpdate(double elapsedTime, BYTE flag) = 0;
	virtual void OnMouseWheel(float mouseWheelData) = 0;
	virtual void OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2) = 0;
	virtual void OnSpace() = 0;

};