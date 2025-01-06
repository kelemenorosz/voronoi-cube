#include <resource.h>

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

Resource::Resource(mWRL::ComPtr<ID3D12Device2> device, std::shared_ptr<CommandQueue> copyCQ, BYTE creationFlag, D3D12_RESOURCE_STATES resourceState,
	D3D12_RESOURCE_DIMENSION dimension, UINT64 width, UINT height, UINT64 alignment, UINT16 depthOrArraySize,
	UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount, UINT sampleQuality,
	D3D12_TEXTURE_LAYOUT layout, D3D12_RESOURCE_FLAGS flags, D3D12_CLEAR_VALUE* clearValue) : m_copyCQ(copyCQ), m_device(device) {

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resourceDescription = {};
	resourceDescription.Dimension = dimension;
	resourceDescription.Alignment = alignment;
	resourceDescription.Width = width;
	resourceDescription.Height = height;
	resourceDescription.DepthOrArraySize = depthOrArraySize;
	resourceDescription.MipLevels = mipLevels;
	resourceDescription.Format = format;
	resourceDescription.SampleDesc.Count = sampleCount;
	resourceDescription.SampleDesc.Quality = sampleQuality;
	resourceDescription.Layout = layout;
	resourceDescription.Flags = flags;

	//RESOURCE_NO_READBACK flag disabled
	if (!(creationFlag & 0b00000001)) {

		heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COPY_DEST, clearValue, IID_PPV_ARGS(&m_readback)));

	}

	//RESOURCE_NO_UPLOAD flag disabled
	if (!(creationFlag & 0b00000010)) {

		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

		if (dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {

			ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ, clearValue, IID_PPV_ARGS(&m_upload)));

		}
		else if (dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {

			resourceDescription.Width = width * height * 4;
			resourceDescription.Height = 1;
			resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
			resourceDescription.MipLevels = 1;
			resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
			ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ, clearValue, IID_PPV_ARGS(&m_upload)));

		}

	}

	//RESOURCE_NO_DEFAULT flag disabled
	if (!(creationFlag & 0b00000100)) {

		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		resourceDescription.Dimension = dimension;
		resourceDescription.Width = width;
		resourceDescription.Height = height;
		resourceDescription.Format = format;
		resourceDescription.MipLevels = mipLevels;
		resourceDescription.Layout = layout;
		resourceDescription.Flags = flags;
		ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, resourceState, clearValue, IID_PPV_ARGS(&m_default)));

	}

}

Resource::~Resource() {



}

void Resource::MapUploadBufferPtr(UINT subresource, void** bufferPtr) {

	if (m_upload == nullptr) {

		*bufferPtr = nullptr;
		return;

	}

	ThrowIfFailed(m_upload->Map(subresource, nullptr, bufferPtr));

}

void Resource::UnmapUploadBufferPtr(UINT subresource, D3D12_RANGE* range) {

	m_upload->Unmap(subresource, range);

}

void Resource::CopyUploadToDefault() {

	D3D12_RESOURCE_DESC defaultBufferDescription = m_default->GetDesc();

	if (defaultBufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER) {

		throw std::exception();

	}

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();
	commandList->CopyResource(m_default.Get(), m_upload.Get());
	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

}

void Resource::CopyUploadToDefault(UINT subresource) {

	D3D12_RESOURCE_DESC defaultBufferDescription = m_default->GetDesc();
	
	if (defaultBufferDescription.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D) {

		throw std::exception();

	}

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* placedSubresourceFootprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[defaultBufferDescription.MipLevels];
	m_device->GetCopyableFootprints(&defaultBufferDescription, 0, defaultBufferDescription.MipLevels, 0, placedSubresourceFootprints, nullptr, nullptr, nullptr);

	D3D12_TEXTURE_COPY_LOCATION destinationTextureCopyLocation = {};
	D3D12_TEXTURE_COPY_LOCATION sourceTextureCopyLocation = {};

	destinationTextureCopyLocation.pResource = m_default.Get();
	destinationTextureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destinationTextureCopyLocation.SubresourceIndex = subresource;

	sourceTextureCopyLocation.pResource = m_upload.Get();
	sourceTextureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	sourceTextureCopyLocation.PlacedFootprint = placedSubresourceFootprints[subresource];

	commandList->CopyTextureRegion(&destinationTextureCopyLocation, 0, 0, 0, &sourceTextureCopyLocation, nullptr);

	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

	delete[] placedSubresourceFootprints;

}

D3D12_GPU_VIRTUAL_ADDRESS Resource::GetDefaultGPUVirtualAddress() {

	if (m_default == nullptr) {
	
		throw std::exception();

	}
	
	return m_default->GetGPUVirtualAddress();

}

mWRL::ComPtr<ID3D12Resource> Resource::GetDefaultBuffer() {

	return m_default;

}
