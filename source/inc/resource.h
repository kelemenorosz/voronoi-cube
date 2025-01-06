#pragma once

#include <d3d12.h>

#include <wrl.h>
#include <memory>

#include <commandqueue.h>

class Resource {


public:

	Resource(Microsoft::WRL::ComPtr<ID3D12Device2> device, std::shared_ptr<CommandQueue> copyCQ, BYTE creationFlag, D3D12_RESOURCE_STATES resourceState,
		D3D12_RESOURCE_DIMENSION dimension, UINT64 width, UINT height, UINT64 alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, UINT16 depthOrArraySize = 1,
		UINT16 mipLevels = 1, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, UINT sampleCount = 1, UINT sampleQuality = 0,
		D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_CLEAR_VALUE *clearValue = nullptr);
	~Resource();
	void MapUploadBufferPtr(UINT subresource,void** bufferPtr);
	void UnmapUploadBufferPtr(UINT subresource, D3D12_RANGE* range);
	void CopyUploadToDefault();
	void CopyUploadToDefault(UINT subresource);
	D3D12_GPU_VIRTUAL_ADDRESS GetDefaultGPUVirtualAddress();
	Microsoft::WRL::ComPtr<ID3D12Resource> GetDefaultBuffer();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> m_default = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_upload = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_readback = nullptr;

	std::shared_ptr<CommandQueue> m_copyCQ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device2> m_device = nullptr;

};