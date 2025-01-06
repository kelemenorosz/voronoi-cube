#pragma once

#include <d3d12.h>

#include <wrl.h>

class RootSignature {


public:


	RootSignature(Microsoft::WRL::ComPtr<ID3D12Device2>, D3D12_FEATURE_DATA_ROOT_SIGNATURE*, D3D12_ROOT_PARAMETER1*, UINT, D3D12_ROOT_SIGNATURE_FLAGS);
	~RootSignature();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature();


private:


	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;


};







