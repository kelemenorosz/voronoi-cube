#include <rootsignature.h>

#include <d3dx12.h>

//Link DirectX libraries
#pragma comment(lib, "d3d12.lib")

//DirectX includes
#include <d3d12.h>

//Project external includes
#include <wrl.h>

//Project internal includes
#include <config.h>
#include <helper.h>

namespace mWRL = Microsoft::WRL;

RootSignature::RootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_FEATURE_DATA_ROOT_SIGNATURE* rootSignatureVersion,
	D3D12_ROOT_PARAMETER1* rootParameter, UINT parameterCount, D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags) {

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(parameterCount, rootParameter, 0, NULL, rootSignatureFlags);

	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion->HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(device->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

}

RootSignature::~RootSignature() {



}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature() {

	return m_rootSignature;

}

