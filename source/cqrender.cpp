#include <cqrender.h>

#include <d3dx12.h>

//Link DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "D3DCompiler.lib")

//Link Media Foundation libraries
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

//Media Foundation includes
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

//DirectX includes
#include <d3d12.h>
#include <d3dcompiler.h>
#include <directxmath.h>

//Project external includes
#include <memory>
#include <ctime>

//Project internal includes
#include <helper.h>
#include <config.h>
#include <commandqueue.h>
#include <resolver.h>

namespace mWRL = Microsoft::WRL;

void CQRender::LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription) {

	//Define viewport and scissor rectangle

	//Non-quantized video frame pixel color visualizer | uppper right corner
	m_viewport.TopLeftX = static_cast<FLOAT>(WINDOW_WIDTH) / 2.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(WINDOW_WIDTH) / 2.0f;
	m_viewport.Height = static_cast<FLOAT>(WINDOW_HEIGHT) / 2.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	
	//Color quantized video frame pixel color, centroids visualizer | lower right corner
	m_cubePointViewport.TopLeftX = static_cast<FLOAT>(WINDOW_WIDTH) / 2.0f;
	m_cubePointViewport.TopLeftY = static_cast<FLOAT>(WINDOW_HEIGHT) / 2.0f;;
	m_cubePointViewport.Width = static_cast<FLOAT>(WINDOW_WIDTH) / 2.0f;
	m_cubePointViewport.Height = static_cast<FLOAT>(WINDOW_HEIGHT) / 2.0f;
	m_cubePointViewport.MinDepth = 0.0f;
	m_cubePointViewport.MaxDepth = 1.0f;

	m_scissorRectangle.left = 0;
	m_scissorRectangle.top = 0;
	m_scissorRectangle.right = LONG_MAX;
	m_scissorRectangle.bottom = LONG_MAX;

	//Get video fps from resolver
	m_resolver->GetFPS(updateFPS);
	*updateFPS = 1.0f / *updateFPS;
	
	//Get frame characteristics from resolver
	UINT64 videoWidth = 0;
	UINT64 videoHeight = 0;
	UINT64 videoStride = 0;
	m_resolver->GetVideoFrameMetrics(&videoWidth, &videoHeight, &videoStride);

	UINT64 resourceWidth = videoStride * 720;
	m_sampleResourceWidth = resourceWidth;

	//Create resources for sample data
	//Create resource on CPU accessible memory
	{
		//Create heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		//Create resource description
		D3D12_RESOURCE_DESC DSResourceDescription = {};
		DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		DSResourceDescription.Alignment = 0;
		DSResourceDescription.Width = resourceWidth;
		DSResourceDescription.Height = 1;
		DSResourceDescription.DepthOrArraySize = 1;
		DSResourceDescription.MipLevels = 1;
		DSResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		DSResourceDescription.SampleDesc = { 1, 0 };
		DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		//Create resource
		ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_sampleCopyBuffer)));
	}

	//Create resource on GPU accessible memory
	{
		//Create heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		//Create resource description
		D3D12_RESOURCE_DESC DSResourceDescription = {};
		DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		DSResourceDescription.Alignment = 0;
		DSResourceDescription.Width = resourceWidth;
		DSResourceDescription.Height = 1;
		DSResourceDescription.DepthOrArraySize = 1;
		DSResourceDescription.MipLevels = 1;
		DSResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		DSResourceDescription.SampleDesc = { 1, 0 };
		DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		//Create resource
		ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_sampleVertexBuffer)));
	}

	//Create resource on CPU accessible memory - upload to back buffer
	{
		//Create heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		//Create resource description
		D3D12_RESOURCE_DESC DSResourceDescription = {};
		DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		DSResourceDescription.Alignment = 0;
		DSResourceDescription.Width = resourceWidth;
		DSResourceDescription.Height = 1;
		DSResourceDescription.DepthOrArraySize = 1;
		DSResourceDescription.MipLevels = 1;
		DSResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		DSResourceDescription.SampleDesc = { 1, 0 };
		DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		//Create resource
		ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_sampleCopyBackBuffer)));
	
		//Query for subresouce footprint
		m_dxDevice->GetCopyableFootprints(&backBufferDescription, 0, 1, 0, &m_sampleSubresourceFootprint, nullptr, nullptr, nullptr);
	
	}

	//Initialize compute shader resources
	InitComputeShaderResources(1280, 720);

	//Initialize compute shader root signature and pipeline state object
	InitComputeShaderRSAndPSO();

	//Initialize kc compute shader resources
	InitCSKC1Resources(1280, 720);

	//Initialize kc compute shader root signature and pipeline state object
	InitCSKC1RSAndPSO();
	InitCSKC2RSAndPSO();
	InitCSKC3RSAndPSO();

	//Create sample vertex buffer view
	m_sampleVBView.BufferLocation = m_sampleVertexBuffer->GetGPUVirtualAddress();
	m_sampleVBView.SizeInBytes = static_cast<UINT>(resourceWidth);
	m_sampleVBView.StrideInBytes = 4; //4 bytes per pixel

	IMFSample* testSample = nullptr;

	//Skip first SKIP_FRAME_COUNT video frames
	for (UINT i = 0; i < SKIP_FRAME_COUNT; ++i) {

		m_resolver->StartGetSample();
		m_resolver->EndGetSample(&testSample);
		SafeRelease(&testSample);

	}

	//Upload first sample to GPU buffer
	m_resolver->StartGetSample();
	m_resolver->EndGetSample(&testSample);

	UploadSampleToGPU(testSample, m_sampleCopyBuffer.Get(), m_sampleVertexBuffer.Get());
	LoadCopyBuffer(testSample, m_sampleCopyBackBuffer.Get());

	SafeRelease(&testSample);

	//Start getting the next sample
	m_resolver->StartGetSample();

	//Init the pipeline
	InitRootSignature();
	InitPSO();
	InitPointPSO();
	InitDS();
	InitIABuffer(reinterpret_cast<void*>(m_pixelData), _countof(m_pixelData) * sizeof(DirectX::XMFLOAT4), &m_pixelVertexBuffer,
		reinterpret_cast<void*>(&m_pixelVBView), static_cast<UINT>(sizeof(DirectX::XMFLOAT4)));													//Load the pixel vertex buffer
	InitIABuffer(reinterpret_cast<void*>(m_colorData), _countof(m_colorData) * sizeof(DirectX::XMFLOAT4), &m_colorVertexBuffer,
		reinterpret_cast<void*>(&m_colorVBView), static_cast<UINT>(sizeof(DirectX::XMFLOAT4)));													//Load the color vertex buffer
	InitIABuffer(reinterpret_cast<void*>(m_indexData), _countof(m_indexData) * sizeof(WORD), &m_indexBuffer,
		reinterpret_cast<void*>(&m_IBView), NULL, DXGI_FORMAT_R16_UINT);																		//Load the index buffer
	InitCentroidCubePointVertexBuffer();
	InitCentroidCubePointRSAndPSO();

	m_isLoaded = TRUE;

}

BOOL CQRender::OnRender(mWRL::ComPtr<ID3D12GraphicsCommandList> commandList, mWRL::ComPtr<ID3D12Resource> backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE backBufferDescriptor) {

	//If LoadContent is not finished exit
	if (!m_isLoaded) return FALSE;

	//Compute shader - downsample non-quantized image
	
	//Set downsample input image
	{
		//Set SRV resource state to D3D12_RESOURCE_STATE_COPY_DEST
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureInput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Copy into SRV resource - input image
		D3D12_TEXTURE_COPY_LOCATION destinationCopyLocation = {};
		D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation = {};

		destinationCopyLocation.pResource = m_csPictureInput.Get();
		destinationCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destinationCopyLocation.SubresourceIndex = 0;

		sourceCopyLocation.pResource = m_sampleCopyBackBuffer.Get();
		sourceCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceCopyLocation.PlacedFootprint = m_csPictureInputFootprint;

		commandList->CopyTextureRegion(&destinationCopyLocation, 0, 0, 0, &sourceCopyLocation, NULL);

		//Set SRV resource state to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureInput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}
	}

	//Bind pipeline state and root signature
	commandList->SetPipelineState(m_csPSO.Get());
	commandList->SetComputeRootSignature(m_csRootSignature.Get());

	//Bind descriptor heaps
	ID3D12DescriptorHeap* descriptorHeaps[2] = { m_SRVUAVDescriptorHeap.Get(), m_SamplerDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//Bind root signature parameters
	commandList->SetComputeRootDescriptorTable(0, m_SRVUAVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	commandList->SetComputeRootDescriptorTable(1, m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//Call dispatch
	commandList->Dispatch(1280 / 2, 720 / 2, 1);

	//Render pipeline

	//Get depth stencil view
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//Set back buffer state to D3D12_RESOURCE_STATE_RENDER_TARGET
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = backBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	//Clear the back buffer and the depth-stencil buffer
	FLOAT backBufferClearValue[] = {1.0f, 1.0f, 1.0f, 1.0f};
	D3D12_RECT clearRectangle = {WINDOW_WIDTH / 2, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	//commandList->ClearRenderTargetView(backBufferDescriptor, backBufferClearValue, 0, NULL);						//Back buffer clear
	commandList->ClearRenderTargetView(backBufferDescriptor, backBufferClearValue, 1, &clearRectangle);				//Back buffer clear
	commandList->ClearDepthStencilView(depthStencilDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);			//Depth stencil buffer clear

	//Copy sample buffer to back buffer - non-quantized image
	{
		//Set back buffer state to D3D12_RESOURCE_STATE_COPY_DEST
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = backBuffer.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set UAV resource state to D3D12_RESOURCE_STATE_COPY_SOURCE
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureOutput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Copy from copy buffer to back buffer
		D3D12_TEXTURE_COPY_LOCATION textureDestination = {};
		D3D12_TEXTURE_COPY_LOCATION textureSource = {};

		textureDestination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; //Resource must be a texture
		textureDestination.pResource = backBuffer.Get();
		textureDestination.SubresourceIndex = 0;

		textureSource.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; //Resource must be a texture
		textureSource.pResource = m_csPictureOutput.Get();
		textureSource.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&textureDestination, 0, 0, 0, &textureSource, NULL);

		//Set UAV resource state to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureOutput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set back buffer state to D3D12_RESOURCE_STATE_RENDER_TARGET
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = backBuffer.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}
	}

	//Compute shader - downsample color quantized image

	//Set downsample input image
	{
		//Set SRV resource state to D3D12_RESOURCE_STATE_COPY_DEST
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureInput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set quantized image resource state to D3D12_RESOURCE_STATE_COPY_SOURCE
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_cskc1PictureTexture.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Copy into SRV resource - input image
		D3D12_TEXTURE_COPY_LOCATION destinationCopyLocation = {};
		D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation = {};

		destinationCopyLocation.pResource = m_csPictureInput.Get();
		destinationCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destinationCopyLocation.SubresourceIndex = 0;

		sourceCopyLocation.pResource = m_cskc1PictureTexture.Get();
		sourceCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		sourceCopyLocation.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&destinationCopyLocation, 0, 0, 0, &sourceCopyLocation, NULL);

		//Set quantized image resource state to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_cskc1PictureTexture.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set SRV resource state to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureInput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}
	}

	//Bind descriptor heaps
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//Bind root signature parameters
	commandList->SetComputeRootDescriptorTable(0, m_SRVUAVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	commandList->SetComputeRootDescriptorTable(1, m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//Call dispatch - downsample quantized image
	commandList->Dispatch(1280 / 2, 720 / 2, 1);

	//Copy sample buffer to back buffer - color quantized image
	{
		//Set back buffer state to D3D12_RESOURCE_STATE_COPY_DEST
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = backBuffer.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set UAV resource state to D3D12_RESOURCE_STATE_COPY_SOURCE
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureOutput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Copy from copy buffer to back buffer
		D3D12_TEXTURE_COPY_LOCATION textureDestination = {};
		D3D12_TEXTURE_COPY_LOCATION textureSource = {};

		textureDestination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; //Resource must be a texture
		textureDestination.pResource = backBuffer.Get();
		textureDestination.SubresourceIndex = 0;

		textureSource.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; //Resource must be a texture
		textureSource.pResource = m_csPictureOutput.Get();
		textureSource.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&textureDestination, 0, WINDOW_HEIGHT / 2, 0, &textureSource, NULL);

		//Set UAV resource state to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_csPictureOutput.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}

		//Set back buffer state to D3D12_RESOURCE_STATE_RENDER_TARGET
		{
			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = backBuffer.Get();
			resourceBarrier.Transition.Subresource = 0;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}
	}

	//First pass

	//Bind pipeline state and root signature
	commandList->SetPipelineState(m_pointPSO.Get());
	commandList->SetGraphicsRootSignature(m_constantRootSignature.Get());

	//Set up for the Input Assembler
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] = {

			m_sampleVBView,

		};
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);					//Set primitive topology
		commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);		//Set vertex buffers
	}

	//Set root signature parameters
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

	//Set viewport and scissor rectangle
	commandList->RSSetViewports(1, &m_viewport);
	commandList->RSSetScissorRects(1, &m_scissorRectangle);

	//Set back buffer as render target
	commandList->OMSetRenderTargets(1, &backBufferDescriptor, FALSE, &depthStencilDescriptor);

	//Issue draw command
	commandList->DrawInstanced(921600, 1, 0, 0);

	//Second pass

	//Bind pipeline state and root signature
	commandList->SetPipelineState(m_PSO.Get());
	commandList->SetGraphicsRootSignature(m_constantRootSignature.Get());
	
	//Set up for the Input Assembler
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] = {

			m_pixelVBView,
			m_colorVBView

		};
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);					//Set primitive topology
		commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);		//Set vertex buffers
		commandList->IASetIndexBuffer(&m_IBView);												//Set index buffer
	}

	//Set root signature parameters
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

	//Set back buffer as render target
	commandList->OMSetRenderTargets(1, &backBufferDescriptor, FALSE, &depthStencilDescriptor);

	//Set scissor rectangle
	commandList->RSSetScissorRects(1, &m_scissorRectangle);
	
	//Set viewport and issue draw command - non-quantized visualizer
	commandList->RSSetViewports(1, &m_viewport);
	commandList->DrawIndexedInstanced(_countof(m_indexData), 1, 0, 0, 0);

	//Set viewport and issue draw command - color quantized visualizer
	//commandList->RSSetViewports(1, &m_cubePointViewport);
	//commandList->DrawIndexedInstanced(_countof(m_indexData), 1, 0, 0, 0);

	//Third pass

	commandList->SetPipelineState(m_cubePointPSO.Get());
	commandList->SetGraphicsRootSignature(m_cubePointRootSignature.Get());

	//Set up for the Input Assembler
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] = {

			m_cubePointVBView,

		};
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);					//Set primitive topology
		commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);		//Set vertex buffers

	}

	D3D12_GPU_DESCRIPTOR_HANDLE firstDescriptorHandle = m_cskc1SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorHandleIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	firstDescriptorHandle.ptr = SIZE_T(UINT64(firstDescriptorHandle.ptr) + UINT64(descriptorHandleIncrementSize));
	ID3D12DescriptorHeap* thirdPassDescriptorHeaps[1] = { m_cskc1SRVDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(thirdPassDescriptorHeaps), thirdPassDescriptorHeaps);

	commandList->SetGraphicsRootDescriptorTable(0, firstDescriptorHandle);
	commandList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

	commandList->OMSetRenderTargets(1, &backBufferDescriptor, FALSE, &depthStencilDescriptor);

	commandList->RSSetScissorRects(1, &m_scissorRectangle);
	commandList->RSSetViewports(1, &m_cubePointViewport);

	commandList->DrawInstanced(1000000, 1, 0, 0);

	//Fourth pass

	//Bind pipeline state and root signature
	commandList->SetPipelineState(m_pointPSO.Get());
	commandList->SetGraphicsRootSignature(m_constantRootSignature.Get());

	//Set up for the Input Assembler
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] = {

			m_sampleVBView,

		};
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);					//Set primitive topology
		commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);		//Set vertex buffers
	}

	//Set root signature parameters
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_viewProjectionMatrix, 0);

	//Set viewport and scissor rectangle
	commandList->RSSetViewports(1, &m_cubePointViewport);
	commandList->RSSetScissorRects(1, &m_scissorRectangle);

	//Set back buffer as render target
	commandList->OMSetRenderTargets(1, &backBufferDescriptor, FALSE, &depthStencilDescriptor);

	//Issue draw command
	//commandList->DrawInstanced(921600, 1, 0, 0);

	//Set back buffer state to D3D12_RESOURCE_STATE_PRESENT
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = backBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	return TRUE;

}

BOOL CQRender::OnUpdate(double elapsedTime, BYTE flag) {

	//If LoadContent is not finished exit
	if (!m_isLoaded) return FALSE;

	if (flag & CQRENDER_ONUPDATE_UPDATE_SAMPLE_BUFFER) {
		
		IMFSample* testSample = nullptr;
		m_resolver->EndGetSample(&testSample);

		//Vertex buffer for point topology
		UploadSampleToGPU(testSample, m_sampleCopyBuffer.Get(), m_sampleVertexBuffer.Get());
		
		//Copy buffer
		LoadCopyBuffer(testSample, m_sampleCopyBackBuffer.Get());

		//Set initial centroid colors
		InitCentroidColors(testSample);

		SafeRelease(&testSample);

		//Start getting next sample
		m_resolver->StartGetSample();

		//Compute shader for k-means clustering
		InitCentroidBuffer();
		InitTextureBuffer();

		CSKC1(); //First k-means clustering shader
		CSKC2(); //Second k-means clustering shader
		
		ReadbackCentroidBuffer();

	}

	return TRUE;

}

void CQRender::OnMouseWheel(float mouseWheelData) {}

void CQRender::OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2) {

	//Get unit vectors
	DirectX::XMVECTOR v1 = this->GetUnitVector(mX1, mY1);
	DirectX::XMVECTOR v2 = this->GetUnitVector(mX2, mY2);

	//Get rotation angle
	DirectX::XMVECTOR rotationAngleVector = DirectX::XMVector3Dot(v1, v2);
	FLOAT rotationAngle = DirectX::XMVectorGetX(rotationAngleVector);
	rotationAngle = min(1.0f, acosf(rotationAngle)) * 2.0f;

	//Get rotation axis
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(v1, v2);

	//Set view matrix
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, rotationAngle);
	rotationMatrix = DirectX::XMMatrixInverse(nullptr, rotationMatrix);
	m_viewMatrix = DirectX::XMMatrixMultiply(rotationMatrix, m_viewMatrix);

	//Update the view projection matrix
	m_viewProjectionMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, m_projectionMatrix);

}

void CQRender::OnSpace() {



}

CQRender::CQRender(Microsoft::WRL::ComPtr<ID3D12Device2> dxDevice, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ) 
	: m_dxDevice(dxDevice), m_copyCQ(copyCQ), m_directCQ(directCQ), m_isLoaded(FALSE), m_sampleResourceWidth(0) {

	m_pixelVBView = { 0 };
	m_colorVBView = { 0 };
	m_IBView = { 0 };
	m_viewport = { 0 };
	m_cubePointViewport = { 0 };
	m_scissorRectangle = { 0 };
	m_sampleVBView = { 0 };
	m_sampleSubresourceFootprint = { 0 };
	m_csPictureInputFootprint = { 0 };
	m_cskc1PictureTextureFootprint = { 0 };
	m_cubePointVBView = { 0 };

	std::srand(static_cast<UINT>(std::time(nullptr)));

	m_resolver = std::make_unique<Resolver>(INPUT_VIDEO_FILE_PATH);

	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0.0f, 0.0f, -2.7f, 1.0f);
	const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	//const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(15.0f));
	//const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(15.0f), DirectX::XMConvertToRadians(30.0f), 0.0f);
	//m_viewMatrix = DirectX::XMMatrixMultiply(rotationMatrix, m_viewMatrix);

	const float aspectRatio = ((float)WINDOW_WIDTH / 2.0f) / ((float)WINDOW_HEIGHT / 2.0f);
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), aspectRatio, 0.1f, 100.0f);

	m_viewProjectionMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, m_projectionMatrix);

}

CQRender::~CQRender() {}



//Load shaders from disk
void CQRender::LoadShaders() {

	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_vertex.cso", &m_vertexShaderBlob)); //Vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_pixel.cso", &m_pixelShaderBlob)); //Pixel shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_point_vertex.cso", &m_pointVertexShaderBlob)); //Point vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_point_pixel.cso", &m_pointPixelShaderBlob)); //Point pixel shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_compute.cso", &m_csBlob)); //Compute shader
	//ThrowIfFailed(D3DReadFileToBlob(L"./x64/Debug/cqrender_kc1_compute.cso", &m_cskc1Blob)); //K-means clustering (1) compute shader
	//ThrowIfFailed(D3DReadFileToBlob(L"./x64/Debug/cqrender_kc2_compute.cso", &m_cskc2Blob)); //K-means clustering (2) compute shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_kc3_compute.cso", &m_cskc3Blob)); //K-means clustering (3) compute shader
	//ThrowIfFailed(D3DReadFileToBlob(L"./x64/Debug/cqrender_centroid_vertex.cso", &m_cubePointVertexBlob)); //Centroid visualization vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cqrender_centroid_pixel.cso", &m_cubePointPixelBlob)); //Centroid visualization pixel shader

	//Compile shaders at runtime
	char centroidCountBuffer[8] = {};
	char textureWidthBuffer[8] = {};
	char textureHeightBuffer[8] = {};
	sprintf_s(centroidCountBuffer, 8, "%d", COMPUTE_SHADER_KC_CENTROID_COUNT);
	sprintf_s(textureWidthBuffer, 8, "%d", 1280);
	sprintf_s(textureHeightBuffer, 8, "%d", 720);
	D3D_SHADER_MACRO shaderMacros[4] = {};
	shaderMacros[0].Name = "CENTROID_COUNT";
	shaderMacros[0].Definition = centroidCountBuffer;
	shaderMacros[1].Name = "TEXTURE_WIDTH";
	shaderMacros[1].Definition = textureWidthBuffer;
	shaderMacros[2].Name = "TEXTURE_HEIGHT";
	shaderMacros[2].Definition = textureHeightBuffer;
	shaderMacros[3].Name = NULL;
	shaderMacros[3].Definition = NULL;

	ID3DBlob* errorBlob = nullptr;
	//ThrowIfFailed(D3DCompileFromFile(L"./cqrender_kc1_compute.hlsl", shaderMacros, NULL, "main", "cs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &m_cskc1Blob, &errorBlob));
	D3DCompileFromFile(L"./source/shaders/cqrender_kc1_compute.hlsl", shaderMacros, NULL, "main", "cs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &m_cskc1Blob, &errorBlob);
	if (errorBlob) {
	
		char* errorBlobBits = nullptr;
		errorBlobBits = (char*)errorBlob->GetBufferPointer();
		SafeRelease(&errorBlob);
		throw std::exception();
	
	}
	D3DCompileFromFile(L"./source/shaders/cqrender_kc2_compute.hlsl", shaderMacros, NULL, "main", "cs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &m_cskc2Blob, &errorBlob);
	if (errorBlob) {

		char* errorBlobBits = nullptr;
		errorBlobBits = (char*)errorBlob->GetBufferPointer();
		SafeRelease(&errorBlob);
		throw std::exception();

	}
	D3DCompileFromFile(L"./source/shaders/cqrender_centroid_vertex.hlsl", shaderMacros, NULL, "main", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &m_cubePointVertexBlob, &errorBlob);
	if (errorBlob) {

		char* errorBlobBits = nullptr;
		errorBlobBits = (char*)errorBlob->GetBufferPointer();
		SafeRelease(&errorBlob);
		throw std::exception();

	}

}

//Unload shaders
void CQRender::UnloadShaders() {

	m_vertexShaderBlob = nullptr;
	m_pixelShaderBlob = nullptr;
	m_pointVertexShaderBlob = nullptr;
	m_pointPixelShaderBlob = nullptr;
	m_csBlob = nullptr;
	m_cskc1Blob = nullptr;
	m_cskc2Blob = nullptr;
	m_cskc3Blob = nullptr;
	m_cubePointVertexBlob = nullptr;
	m_cubePointPixelBlob = nullptr;
}

//Initialize Root Signature
void CQRender::InitRootSignature() {

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create empty root signature
	{
		//Create root signature description
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
		rootSignatureDescription.Init_1_1(0, NULL, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//Create root signature
		ID3D10Blob* serializedRootSignatureBlob = nullptr;
		ID3D10Blob* errorBlob = nullptr;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
		ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_emptyRootSignature)));

		SafeRelease(&serializedRootSignatureBlob);
		SafeRelease(&errorBlob);
	}

	//Create root signature with one constant
	{
	
		//Create root signature parameter
		D3D12_ROOT_PARAMETER1 rootParameter = {};
		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParameter.Constants.ShaderRegister = 0;
		rootParameter.Constants.RegisterSpace = 0;
		rootParameter.Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		CD3DX12_ROOT_PARAMETER1 cxrootParameter = {};
		cxrootParameter.InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		//Create root signature description
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
		//rootSignatureDescription.Init_1_1(1, &rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDescription.Init_1_1(1, &cxrootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//Create root signature
		ID3D10Blob* serializedRootSignatureBlob = nullptr;
		ID3D10Blob* errorBlob = nullptr;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
		ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_constantRootSignature)));

		SafeRelease(&serializedRootSignatureBlob);
		SafeRelease(&errorBlob);
	
	}
	
}

//Initialize Pipeline State Object
void CQRender::InitPSO() {

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT PSOInputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PSOPrimitiveTopology;
		CD3DX12_PIPELINE_STATE_STREAM_VS PSOVertexShader;
		CD3DX12_PIPELINE_STATE_STREAM_PS PSOPixelShader;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS PSORenderTargetFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT PSODepthStencilFormat;

	} PSOSubStreams;

	//Set input layout for the Input Assembler 
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescriptionElements[] = {
	
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	
	};

	inputLayoutDescription.pInputElementDescs = inputLayoutDescriptionElements;
	inputLayoutDescription.NumElements = _countof(inputLayoutDescriptionElements);
	PSOSubStreams.PSOInputLayout = inputLayoutDescription;

	//Set root signature
	//PSOSubStreams.PSORootSignature = m_emptyRootSignature.Get();
	PSOSubStreams.PSORootSignature = m_constantRootSignature.Get();

	//Set primitive topology
	PSOSubStreams.PSOPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//Load shaders
	LoadShaders();

	//Set loaded shader bytecode
	PSOSubStreams.PSOVertexShader = {m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize()}; //Vertex shader
	PSOSubStreams.PSOPixelShader = {m_pixelShaderBlob->GetBufferPointer(), m_pixelShaderBlob->GetBufferSize()}; //Pixel shader

	//Set depth stencil format
	PSOSubStreams.PSODepthStencilFormat = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;

	//Set render target format
	D3D12_RT_FORMAT_ARRAY renderTargetFormatArray = {};
	DXGI_FORMAT renderTargetFormats[] = {
	
		SWAP_CHAIN_BUFFER_FORMAT

	};

	for (UINT i = 0; i < _countof(renderTargetFormats); ++i) renderTargetFormatArray.RTFormats[i] = renderTargetFormats[i];
	renderTargetFormatArray.NumRenderTargets = _countof(renderTargetFormats);
	PSOSubStreams.PSORenderTargetFormats = renderTargetFormatArray;

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_PSO)));

	//Unload shaders
	UnloadShaders();

}

//Initialize Pipleline State Object for points
void CQRender::InitPointPSO() {

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT PSOInputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PSOPrimitiveTopology;
		CD3DX12_PIPELINE_STATE_STREAM_VS PSOVertexShader;
		CD3DX12_PIPELINE_STATE_STREAM_PS PSOPixelShader;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS PSORenderTargetFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT PSODepthStencilFormat;

	} PSOSubStreams;

	//Set input layout for the Input Assembler 
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescriptionElements[] = {

			{"R_POSITION", 0,  DXGI_FORMAT_R32_UINT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			
	};

	//Set root signature
	inputLayoutDescription.pInputElementDescs = inputLayoutDescriptionElements;
	inputLayoutDescription.NumElements = _countof(inputLayoutDescriptionElements);
	PSOSubStreams.PSOInputLayout = inputLayoutDescription;

	PSOSubStreams.PSORootSignature = m_constantRootSignature.Get();

	//Set primitive topology
	PSOSubStreams.PSOPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	//Load shaders
	LoadShaders();

	//Set loaded shader bytecode
	PSOSubStreams.PSOVertexShader = { m_pointVertexShaderBlob->GetBufferPointer(), m_pointVertexShaderBlob->GetBufferSize() }; //Vertex shader
	PSOSubStreams.PSOPixelShader = { m_pointPixelShaderBlob->GetBufferPointer(), m_pointPixelShaderBlob->GetBufferSize() }; //Pixel shader

	//Set depth stencil format
	PSOSubStreams.PSODepthStencilFormat = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;

	//Set render target format
	D3D12_RT_FORMAT_ARRAY renderTargetFormatArray = {};
	DXGI_FORMAT renderTargetFormats[] = {

		SWAP_CHAIN_BUFFER_FORMAT

	};

	for (UINT i = 0; i < _countof(renderTargetFormats); ++i) renderTargetFormatArray.RTFormats[i] = renderTargetFormats[i];
	renderTargetFormatArray.NumRenderTargets = _countof(renderTargetFormats);
	PSOSubStreams.PSORenderTargetFormats = renderTargetFormatArray;

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_pointPSO)));

	//Unload shaders
	UnloadShaders();

}

//Initialize Root Signature and Pipeline State Object for centroid point visualizer
void CQRender::InitCentroidCubePointRSAndPSO() {

	//Create root signature

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create root signature parameters
	D3D12_ROOT_PARAMETER1 rootParameter[2] = {};
	D3D12_DESCRIPTOR_RANGE1 SRVUAVDescriptorRanges[1] = {};
	SRVUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	SRVUAVDescriptorRanges[0].NumDescriptors = 1;
	SRVUAVDescriptorRanges[0].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[0].RegisterSpace = 0;
	SRVUAVDescriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(SRVUAVDescriptorRanges);
	rootParameter[0].DescriptorTable.pDescriptorRanges = SRVUAVDescriptorRanges;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter[1].Constants.ShaderRegister = 0;
	rootParameter[1].Constants.RegisterSpace = 0;
	rootParameter[1].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Create root signature description
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(_countof(rootParameter), rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//Create root signature
	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_cubePointRootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

	//Create pipeline state object

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT PSOInputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PSOPrimitiveTopology;
		CD3DX12_PIPELINE_STATE_STREAM_VS PSOVertexShader;
		CD3DX12_PIPELINE_STATE_STREAM_PS PSOPixelShader;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS PSORenderTargetFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT PSODepthStencilFormat;

	} PSOSubStreams;

	//Set input layout for the Input Assembler 
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescriptionElements[] = {

			//{"R_POSITION", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"R_POSITION", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};

	//Set root signature
	inputLayoutDescription.pInputElementDescs = inputLayoutDescriptionElements;
	inputLayoutDescription.NumElements = _countof(inputLayoutDescriptionElements);
	PSOSubStreams.PSOInputLayout = inputLayoutDescription;

	PSOSubStreams.PSORootSignature = m_cubePointRootSignature.Get();

	//Set primitive topology
	PSOSubStreams.PSOPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	//Load shaders
	LoadShaders();

	//Set loaded shader bytecode
	PSOSubStreams.PSOVertexShader = { m_cubePointVertexBlob->GetBufferPointer(), m_cubePointVertexBlob->GetBufferSize() }; //Vertex shader
	PSOSubStreams.PSOPixelShader = { m_cubePointPixelBlob->GetBufferPointer(), m_cubePointPixelBlob->GetBufferSize() }; //Pixel shader

	//Set depth stencil format
	PSOSubStreams.PSODepthStencilFormat = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;

	//Set render target format
	D3D12_RT_FORMAT_ARRAY renderTargetFormatArray = {};
	DXGI_FORMAT renderTargetFormats[] = {

		SWAP_CHAIN_BUFFER_FORMAT

	};

	for (UINT i = 0; i < _countof(renderTargetFormats); ++i) renderTargetFormatArray.RTFormats[i] = renderTargetFormats[i];
	renderTargetFormatArray.NumRenderTargets = _countof(renderTargetFormats);
	PSOSubStreams.PSORenderTargetFormats = renderTargetFormatArray;

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_cubePointPSO)));

	//Unload shaders
	UnloadShaders();

}

//Initialize vertex buffer for centroid point visualizer
void CQRender::InitCentroidCubePointVertexBuffer() {

	//Create CPU accessible copy buffer
	D3D12_HEAP_PROPERTIES cubePointCopyBufferHeapProperties = {};
	cubePointCopyBufferHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	cubePointCopyBufferHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	cubePointCopyBufferHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	cubePointCopyBufferHeapProperties.CreationNodeMask = 0;
	cubePointCopyBufferHeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC cubePointCopyBufferResourceDescription = {};
	cubePointCopyBufferResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cubePointCopyBufferResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	cubePointCopyBufferResourceDescription.Width = UINT64(1000000) * UINT64(4); //Million points across the cube - 4 bytes each
	cubePointCopyBufferResourceDescription.Height = 1;
	cubePointCopyBufferResourceDescription.DepthOrArraySize = 1;
	cubePointCopyBufferResourceDescription.MipLevels = 1;
	cubePointCopyBufferResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	cubePointCopyBufferResourceDescription.SampleDesc = { 1, 0 };
	cubePointCopyBufferResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	cubePointCopyBufferResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&cubePointCopyBufferHeapProperties, D3D12_HEAP_FLAG_NONE,
		&cubePointCopyBufferResourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cubePointCopyBuffer)));

	//Create GPU accessible buffer
	D3D12_HEAP_PROPERTIES cubePointBufferHeapProperties = {};
	cubePointBufferHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	cubePointBufferHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	cubePointBufferHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	cubePointBufferHeapProperties.CreationNodeMask = 0;
	cubePointBufferHeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC cubePointBufferResourceDescription = {};
	cubePointBufferResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cubePointBufferResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	cubePointBufferResourceDescription.Width = UINT64(1000000) * UINT64(4); //Million points across the cube
	cubePointBufferResourceDescription.Height = 1;
	cubePointBufferResourceDescription.DepthOrArraySize = 1;
	cubePointBufferResourceDescription.MipLevels = 1;
	cubePointBufferResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	cubePointBufferResourceDescription.SampleDesc = { 1, 0 };
	cubePointBufferResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	cubePointBufferResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&cubePointBufferHeapProperties, D3D12_HEAP_FLAG_NONE,
		&cubePointBufferResourceDescription, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_cubePointVertexBuffer)));

	//Create vertex buffer view for cube point buffer
	m_cubePointVBView.BufferLocation = m_cubePointVertexBuffer->GetGPUVirtualAddress();
	m_cubePointVBView.SizeInBytes = UINT(1000000) * UINT(4);
	m_cubePointVBView.StrideInBytes = 4;

	//Set points into contiguous buffer
	UINT32* cubePoints = new UINT32[1000000];
	*cubePoints = 0x00000000;
	UINT32* cubePointCounter = cubePoints;

	UINT8 cubePointW = 0;

	for (UINT8 cubePointX = 0; cubePointX < 100; ++cubePointX) {

		for (UINT8 cubePointY = 0; cubePointY < 100; ++cubePointY) {

			for (UINT8 cubePointZ = 0; cubePointZ < 100; ++cubePointZ) {

				*cubePointCounter = 0x00000000;
				*cubePointCounter = (*cubePointCounter) | (static_cast<UINT32>(cubePointW) << 24);
				*cubePointCounter = (*cubePointCounter) | (static_cast<UINT32>(cubePointX) << 16);
				*cubePointCounter = (*cubePointCounter) | (static_cast<UINT32>(cubePointY) << 8);
				*cubePointCounter = (*cubePointCounter) | static_cast<UINT32>(cubePointZ);

				cubePointCounter++;

			}

		}

	}

	cubePointCounter--;
	UINT32 cubePoint1 = *(cubePoints + 102);
	//Copy into copy buffer
	BYTE* cubePointCopyBufferBits = nullptr;
	D3D12_RANGE readRange = {0, 0};
	D3D12_RANGE writeRange = {0, SIZE_T(1000000) * SIZE_T(4)};
	ThrowIfFailed(m_cubePointCopyBuffer->Map(0, &readRange, reinterpret_cast<void**>(&cubePointCopyBufferBits)));

	::memcpy(cubePointCopyBufferBits, cubePoints, SIZE_T(1000000) * SIZE_T(4));

	m_cubePointCopyBuffer->Unmap(0, &writeRange);
	
	//Release contiguous buffer
	delete[] cubePoints;

	//Copy into vertex buffer 
	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();

	commandList->CopyBufferRegion(m_cubePointVertexBuffer.Get(), 0, m_cubePointCopyBuffer.Get(), 0, UINT64(1000000) * UINT64(4));

	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

}

//Initialize and load the vertex buffer
void CQRender::InitIABuffer(void* buffer, UINT64 width, ID3D12Resource** destinationBuffer, void* destinationBufferView, UINT stride, DXGI_FORMAT format) {

	//Create resource on CPU accessible memory
	ID3D12Resource* intermediateBuffer = nullptr;

	{
		//Create heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		//Create resource description
		D3D12_RESOURCE_DESC DSResourceDescription = {};
		DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		DSResourceDescription.Alignment = 0;
		DSResourceDescription.Width = width;
		DSResourceDescription.Height = 1;
		DSResourceDescription.DepthOrArraySize = 1;
		DSResourceDescription.MipLevels = 1;
		DSResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		DSResourceDescription.SampleDesc = { 1, 0 };
		DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		//Create resource
		ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&intermediateBuffer)));
	}

	//Create resource on GPU accessible memory
	
	{
		//Create heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		//Create resource description
		D3D12_RESOURCE_DESC DSResourceDescription = {};
		DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		DSResourceDescription.Alignment = 0;
		DSResourceDescription.Width = width;
		DSResourceDescription.Height = 1;
		DSResourceDescription.DepthOrArraySize = 1;
		DSResourceDescription.MipLevels = 1;
		DSResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		DSResourceDescription.SampleDesc = { 1, 0 };
		DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		//Create resource
		ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(destinationBuffer)));
	}

	//Copy from buffer to intermediate
	
	BYTE* intermediateBufferBits = nullptr; //Pointer to intermediate buffer location
	D3D12_RANGE readRange = { 0, 0 }; //CPU not reading, only write operation
	D3D12_RANGE writeRange = { 0, width }; //CPU write
	ThrowIfFailed(intermediateBuffer->Map(0, &readRange, reinterpret_cast<void**>(&intermediateBufferBits)));

	::memcpy(intermediateBufferBits, buffer, static_cast<SIZE_T>(width));

	intermediateBuffer->Unmap(0, &writeRange);

	//Set copy operation from intermediate to destinaton
	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();
	commandList->CopyBufferRegion(*destinationBuffer, 0, intermediateBuffer, 0, width);

	//Execute command list on GPU
	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

	//Release intermediate buffer
	SafeRelease(&intermediateBuffer);

	//Set buffer view
	if (format == DXGI_FORMAT_UNKNOWN) {

		//Vertex Buffer
		D3D12_VERTEX_BUFFER_VIEW* vertexBufferView = reinterpret_cast<D3D12_VERTEX_BUFFER_VIEW*>(destinationBufferView);
		vertexBufferView->BufferLocation = (*destinationBuffer)->GetGPUVirtualAddress();
		vertexBufferView->SizeInBytes = static_cast<UINT>(width);
		vertexBufferView->StrideInBytes = stride;

	}
	else {

		//Index Buffer
		D3D12_INDEX_BUFFER_VIEW* indexBufferView = reinterpret_cast<D3D12_INDEX_BUFFER_VIEW*>(destinationBufferView);
		indexBufferView->BufferLocation = (*destinationBuffer)->GetGPUVirtualAddress();
		indexBufferView->SizeInBytes = static_cast<UINT>(width);
		indexBufferView->Format = format;

	}

}

//Initialize the depth-stencil buffer
void CQRender::InitDS() {

	//Create heap properties
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	//Create resource description
	D3D12_RESOURCE_DESC DSResourceDescription = {};
	DSResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //Depth Stencil uses TEXTURE2D
	DSResourceDescription.Alignment = 0;
	DSResourceDescription.Width = WINDOW_WIDTH;
	DSResourceDescription.Height = WINDOW_HEIGHT;
	DSResourceDescription.DepthOrArraySize = 1;
	DSResourceDescription.MipLevels = 0;
	DSResourceDescription.Format = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;
	DSResourceDescription.SampleDesc = { 1, 0 };
	DSResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DSResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//Set optimal clear value
	D3D12_CLEAR_VALUE optimalClearValue = {};
	optimalClearValue.Format = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;
	optimalClearValue.DepthStencil = { 1.0f, 0 };

	//Create depth stencil buffer
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &DSResourceDescription, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimalClearValue, IID_PPV_ARGS(&m_dsBuffer)));

	//Create DSV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescription = {};
	descriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDescription.NumDescriptors = 1;
	descriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDescription.NodeMask = 0;
	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(&m_DSVDescriptorHeap)));
	
	//Create Depth Stencil View (DSV)
	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDescription = {};
	DSVDescription.Format = CQRENDER_DEPTH_STENCIL_BUFFER_FORMAT;
	DSVDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDescription.Flags = D3D12_DSV_FLAG_NONE;
	DSVDescription.Texture2D.MipSlice = 0;
	m_dxDevice->CreateDepthStencilView(m_dsBuffer.Get(), &DSVDescription, m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

//Initialize compute shader resources
void CQRender::InitComputeShaderResources(UINT64 inputWidth, UINT64 inputHeight) {

	//Create SRV UAV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC SRVUAVDescriptorHeapDescription = {};
	SRVUAVDescriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	SRVUAVDescriptorHeapDescription.NodeMask = 0;
	SRVUAVDescriptorHeapDescription.NumDescriptors = 2; //1 SRV - input image, 1 UAV - output image
	SRVUAVDescriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&SRVUAVDescriptorHeapDescription, IID_PPV_ARGS(&m_SRVUAVDescriptorHeap)));

	//Create sampler descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC samplerDescriptorHeapDescription = {};
	samplerDescriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	samplerDescriptorHeapDescription.NodeMask = 0;
	samplerDescriptorHeapDescription.NumDescriptors = 1; // 1 - bilinear sampler
	samplerDescriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&samplerDescriptorHeapDescription, IID_PPV_ARGS(&m_SamplerDescriptorHeap)));

	//Create SRV resource - input image
	D3D12_HEAP_PROPERTIES inputImageResouceHeapProperties = {};
	inputImageResouceHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	inputImageResouceHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	inputImageResouceHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	inputImageResouceHeapProperties.CreationNodeMask = 0;
	inputImageResouceHeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC inputImageResourceDescription = {};
	inputImageResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	inputImageResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	inputImageResourceDescription.Width = inputWidth;
	inputImageResourceDescription.Height = static_cast<UINT>(inputHeight);
	inputImageResourceDescription.DepthOrArraySize = 1;
	inputImageResourceDescription.MipLevels = 1;
	inputImageResourceDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	inputImageResourceDescription.SampleDesc = { 1, 0 };
	inputImageResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	inputImageResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&inputImageResouceHeapProperties, D3D12_HEAP_FLAG_NONE,
		&inputImageResourceDescription, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&m_csPictureInput)));

	//Get copyable footprint for SRV resource - input picture 
	m_dxDevice->GetCopyableFootprints(&inputImageResourceDescription, 0, 1, 0, &m_csPictureInputFootprint, nullptr, nullptr, nullptr);

	//Create UAV resource - output image
	D3D12_HEAP_PROPERTIES outputImageResourceHeapProperties = {};
	outputImageResourceHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	outputImageResourceHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	outputImageResourceHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	outputImageResourceHeapProperties.CreationNodeMask = 0;
	outputImageResourceHeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC outputImageResourceDescription = {};
	outputImageResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	outputImageResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	outputImageResourceDescription.Width = inputWidth / 2; // Change later to inputWidth / 2
	outputImageResourceDescription.Height = static_cast<UINT>(inputHeight) / 2; // Change later to inputHeight / 2
	outputImageResourceDescription.DepthOrArraySize = 1;
	outputImageResourceDescription.MipLevels = 1;
	outputImageResourceDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	outputImageResourceDescription.SampleDesc = { 1, 0 };
	outputImageResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	outputImageResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&outputImageResourceHeapProperties, D3D12_HEAP_FLAG_NONE, 
		&outputImageResourceDescription, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_csPictureOutput)));

	//Create Shader Resource View
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription = {};
	shaderResourceViewDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	shaderResourceViewDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shaderResourceViewDescription.Texture2D.MipLevels = 1;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescription.Texture2D.PlaneSlice = 0;
	shaderResourceViewDescription.Texture2D.ResourceMinLODClamp = 0.0f;
	m_dxDevice->CreateShaderResourceView(m_csPictureInput.Get(), &shaderResourceViewDescription, m_SRVUAVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//Create Unordered Access View
	D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescription = {};
	unorderedAccessViewDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	unorderedAccessViewDescription.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	unorderedAccessViewDescription.Texture2D.MipSlice = 0;
	unorderedAccessViewDescription.Texture2D.PlaneSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE unorderedAccessViewDescriptorHandle = m_SRVUAVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	UINT UAVDescriptorHandleIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	unorderedAccessViewDescriptorHandle.ptr = SIZE_T(UINT64(unorderedAccessViewDescriptorHandle.ptr) + UINT64(UAVDescriptorHandleIncrementSize));
	m_dxDevice->CreateUnorderedAccessView(m_csPictureOutput.Get(), nullptr, &unorderedAccessViewDescription, unorderedAccessViewDescriptorHandle);

	//Create bilinear sampler
	D3D12_SAMPLER_DESC bilinearSamplerDescription = {};
	bilinearSamplerDescription.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	bilinearSamplerDescription.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	bilinearSamplerDescription.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	bilinearSamplerDescription.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	bilinearSamplerDescription.MipLODBias = 0.0f;
	bilinearSamplerDescription.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	bilinearSamplerDescription.MinLOD = 0;
	bilinearSamplerDescription.MaxLOD = 0;
	m_dxDevice->CreateSampler(&bilinearSamplerDescription, m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

//Initialize compute shader root signature and pipeline state object
void CQRender::InitComputeShaderRSAndPSO() {

	//Create root signature

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create root signature parameters
	D3D12_ROOT_PARAMETER1 rootParameter[2] = {};
	D3D12_DESCRIPTOR_RANGE1 samplerDescriptorRange = {};
	samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	samplerDescriptorRange.NumDescriptors = 1;
	samplerDescriptorRange.BaseShaderRegister = 0;
	samplerDescriptorRange.RegisterSpace = 0;
	samplerDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	samplerDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	D3D12_DESCRIPTOR_RANGE1 SRVUAVDescriptorRanges[2] = {};
	SRVUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVUAVDescriptorRanges[0].NumDescriptors = 1;
	SRVUAVDescriptorRanges[0].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[0].RegisterSpace = 0;
	SRVUAVDescriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	SRVUAVDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	SRVUAVDescriptorRanges[1].NumDescriptors = 1;
	SRVUAVDescriptorRanges[1].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[1].RegisterSpace = 0;
	SRVUAVDescriptorRanges[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(SRVUAVDescriptorRanges);
	rootParameter[0].DescriptorTable.pDescriptorRanges = SRVUAVDescriptorRanges;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //Bilinear sampler
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &samplerDescriptorRange;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Create root signature description
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(_countof(rootParameter), rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);
	
	//Create root signature
	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_csRootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

	//Create pipeline state object

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_CS PSOComputeShader;

	} PSOSubStreams;

	//Set root signature
	PSOSubStreams.PSORootSignature = m_csRootSignature.Get();

	//Load compute shader
	LoadShaders();
	
	//Set compute shader
	PSOSubStreams.PSOComputeShader = { m_csBlob->GetBufferPointer(), m_csBlob->GetBufferSize() };

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_csPSO)));

	//Unload compute shader
	UnloadShaders();

}

//Initialize kc1 compute shader resources 
void CQRender::InitCSKC1Resources(UINT64 inputWidth, UINT64 inputHeight) {

	//Create SRV/UAV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC SRVUAVDescriptorHeapDescription = {};
	SRVUAVDescriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRVUAVDescriptorHeapDescription.NodeMask = 0;
	SRVUAVDescriptorHeapDescription.NumDescriptors = 3;
	SRVUAVDescriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&SRVUAVDescriptorHeapDescription, IID_PPV_ARGS(&m_cskc1SRVDescriptorHeap)));

	//Create GPU accessible texture resource
	D3D12_HEAP_PROPERTIES textureHeapProperties = {};
	textureHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	textureHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	textureHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	textureHeapProperties.CreationNodeMask = 0;
	textureHeapProperties.VisibleNodeMask = 0;
	D3D12_RESOURCE_DESC textureResourceDescription = {};
	textureResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	textureResourceDescription.Width = inputWidth;
	textureResourceDescription.Height = static_cast<UINT>(inputHeight);
	textureResourceDescription.DepthOrArraySize = 1;
	textureResourceDescription.MipLevels = 1;
	textureResourceDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	textureResourceDescription.SampleDesc = { 1, 0 };
	textureResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&textureHeapProperties, D3D12_HEAP_FLAG_NONE, 
		&textureResourceDescription, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_cskc1PictureTexture)));

	//Get copyable footprints for picture texture resource
	m_dxDevice->GetCopyableFootprints(&textureResourceDescription, 0, 1, 0, &m_cskc1PictureTextureFootprint, nullptr, nullptr, nullptr);

	//Create GPU accessible centroid buffer
	D3D12_HEAP_PROPERTIES centroidBufferHeapProperties = {};
	centroidBufferHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	centroidBufferHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	centroidBufferHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	centroidBufferHeapProperties.CreationNodeMask = 0;
	centroidBufferHeapProperties.VisibleNodeMask = 0;
	D3D12_RESOURCE_DESC centroidBufferResourceDescription = {};
	centroidBufferResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	centroidBufferResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	centroidBufferResourceDescription.Width = COMPUTE_SHADER_KC_CENTROID_COUNT * sizeof(Centroid);
	centroidBufferResourceDescription.Height = 1;
	centroidBufferResourceDescription.DepthOrArraySize = 1;
	centroidBufferResourceDescription.MipLevels = 1;
	centroidBufferResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	centroidBufferResourceDescription.SampleDesc = { 1, 0 };
	centroidBufferResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	centroidBufferResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&centroidBufferHeapProperties, D3D12_HEAP_FLAG_NONE, 
		&centroidBufferResourceDescription, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_cskc1CentroidBuffer)));

	//Create CPU accessible centroid copy buffer
	D3D12_HEAP_PROPERTIES centroidCopyBufferHeapProperties = {};
	centroidCopyBufferHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	centroidCopyBufferHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	centroidCopyBufferHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	centroidCopyBufferHeapProperties.CreationNodeMask = 0;
	centroidCopyBufferHeapProperties.VisibleNodeMask = 0;
	D3D12_RESOURCE_DESC centroidCopyBufferResourceDescription = {};
	centroidCopyBufferResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	centroidCopyBufferResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	centroidCopyBufferResourceDescription.Width = COMPUTE_SHADER_KC_CENTROID_COUNT * sizeof(Centroid);
	centroidCopyBufferResourceDescription.Height = 1;
	centroidCopyBufferResourceDescription.DepthOrArraySize = 1;
	centroidCopyBufferResourceDescription.MipLevels = 1;
	centroidCopyBufferResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	centroidCopyBufferResourceDescription.SampleDesc = { 1, 0 };
	centroidCopyBufferResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	centroidCopyBufferResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&centroidCopyBufferHeapProperties, D3D12_HEAP_FLAG_NONE,
		&centroidCopyBufferResourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cskc1CentroidCopyBuffer)));

	//Create CPU accessible readback buffer
	D3D12_HEAP_PROPERTIES centroidReadbackBufferHeapProperties = {};
	centroidReadbackBufferHeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
	centroidReadbackBufferHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	centroidReadbackBufferHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	centroidReadbackBufferHeapProperties.CreationNodeMask = 0;
	centroidReadbackBufferHeapProperties.VisibleNodeMask = 0;
	D3D12_RESOURCE_DESC centroidReadbackBufferResourceDescription = {};
	centroidReadbackBufferResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	centroidReadbackBufferResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	centroidReadbackBufferResourceDescription.Width = COMPUTE_SHADER_KC_CENTROID_COUNT * sizeof(Centroid);
	centroidReadbackBufferResourceDescription.Height = 1;
	centroidReadbackBufferResourceDescription.DepthOrArraySize = 1;
	centroidReadbackBufferResourceDescription.MipLevels = 1;
	centroidReadbackBufferResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	centroidReadbackBufferResourceDescription.SampleDesc = { 1, 0 };
	centroidReadbackBufferResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	centroidReadbackBufferResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
	ThrowIfFailed(m_dxDevice->CreateCommittedResource(&centroidReadbackBufferHeapProperties, D3D12_HEAP_FLAG_NONE,
		&centroidReadbackBufferResourceDescription, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_cskc1CentroidReadbackBuffer)));

	//Get SRV/UAV descriptor heap CPU descriptor handle for heap start
	D3D12_CPU_DESCRIPTOR_HANDLE SRVDescriptorHeapDescriptorHandle = m_cskc1SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//Get descriptor handle increment size
	UINT SRVIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Create texture SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC textureSRVDescription = {};
	textureSRVDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	textureSRVDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	textureSRVDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	textureSRVDescription.Texture2D.MipLevels = 1;
	textureSRVDescription.Texture2D.MostDetailedMip = 0;
	textureSRVDescription.Texture2D.PlaneSlice = 0;
	textureSRVDescription.Texture2D.ResourceMinLODClamp = 0.0f;
	m_dxDevice->CreateShaderResourceView(m_cskc1PictureTexture.Get(), &textureSRVDescription, SRVDescriptorHeapDescriptorHandle);

	//Increment descriptor heap CPU descriptor handle
	SRVDescriptorHeapDescriptorHandle.ptr = SIZE_T(UINT64(SRVDescriptorHeapDescriptorHandle.ptr) + UINT64(SRVIncrementSize));

	//Create centroid buffer UAV
	D3D12_UNORDERED_ACCESS_VIEW_DESC centroidBufferUAVDescription = {};
	centroidBufferUAVDescription.Format = DXGI_FORMAT_UNKNOWN;
	centroidBufferUAVDescription.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	centroidBufferUAVDescription.Buffer.FirstElement = 0;
	centroidBufferUAVDescription.Buffer.NumElements = COMPUTE_SHADER_KC_CENTROID_COUNT;
	centroidBufferUAVDescription.Buffer.StructureByteStride = sizeof(Centroid);
	centroidBufferUAVDescription.Buffer.CounterOffsetInBytes = 0;
	centroidBufferUAVDescription.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	m_dxDevice->CreateUnorderedAccessView(m_cskc1CentroidBuffer.Get(), nullptr, &centroidBufferUAVDescription, SRVDescriptorHeapDescriptorHandle);

	//Increment descriptor heap CPU descriptor handle
	SRVDescriptorHeapDescriptorHandle.ptr = SIZE_T(UINT64(SRVDescriptorHeapDescriptorHandle.ptr) + UINT64(SRVIncrementSize));

	//Create texture UAV
	D3D12_UNORDERED_ACCESS_VIEW_DESC textureUAVDescription = {};
	textureUAVDescription.Format = COMPUTE_SHADER_IMAGE_FORMAT;
	textureUAVDescription.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	textureUAVDescription.Texture2D.MipSlice = 0;
	textureUAVDescription.Texture2D.PlaneSlice = 0;
	m_dxDevice->CreateUnorderedAccessView(m_cskc1PictureTexture.Get(), nullptr, &textureUAVDescription, SRVDescriptorHeapDescriptorHandle);

}

//Initialize kc1 compute shader root signature and pipeline state object
void CQRender::InitCSKC1RSAndPSO() {

	//Create root signature

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create root signature parameters
	D3D12_ROOT_PARAMETER1 rootParameter[1] = {};
	D3D12_DESCRIPTOR_RANGE1 SRVUAVDescriptorRanges[2] = {};
	SRVUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVUAVDescriptorRanges[0].NumDescriptors = 1;
	SRVUAVDescriptorRanges[0].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[0].RegisterSpace = 0;
	SRVUAVDescriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	SRVUAVDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	SRVUAVDescriptorRanges[1].NumDescriptors = 1;
	SRVUAVDescriptorRanges[1].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[1].RegisterSpace = 0;
	SRVUAVDescriptorRanges[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(SRVUAVDescriptorRanges);
	rootParameter[0].DescriptorTable.pDescriptorRanges = SRVUAVDescriptorRanges;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Create root signature description
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(_countof(rootParameter), rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	//Create root signature
	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_cskc1RootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

	//Create pipeline state object

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_CS PSOComputeShader;

	} PSOSubStreams;

	//Set root signature
	PSOSubStreams.PSORootSignature = m_cskc1RootSignature.Get();

	//Load compute shader
	LoadShaders();

	//Set compute shader
	PSOSubStreams.PSOComputeShader = { m_cskc1Blob->GetBufferPointer(), m_cskc1Blob->GetBufferSize() };

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_cskc1PSO)));

	//Unload compute shader
	UnloadShaders();

}

//Initialize kc2 compute shader root signature and pipeline state object
void CQRender::InitCSKC2RSAndPSO() {

	//Create root signature

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create root signature parameters
	D3D12_ROOT_PARAMETER1 rootParameter[1] = {};
	D3D12_DESCRIPTOR_RANGE1 SRVUAVDescriptorRanges[1] = {};
	SRVUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	SRVUAVDescriptorRanges[0].NumDescriptors = 2;
	SRVUAVDescriptorRanges[0].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[0].RegisterSpace = 0;
	SRVUAVDescriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(SRVUAVDescriptorRanges);
	rootParameter[0].DescriptorTable.pDescriptorRanges = SRVUAVDescriptorRanges;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Create root signature description
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(_countof(rootParameter), rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	//Create root signature
	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_cskc2RootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

	//Create pipeline state object

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_CS PSOComputeShader;

	} PSOSubStreams;

	//Set root signature
	PSOSubStreams.PSORootSignature = m_cskc2RootSignature.Get();

	//Load compute shader
	LoadShaders();

	//Set compute shader
	PSOSubStreams.PSOComputeShader = { m_cskc2Blob->GetBufferPointer(), m_cskc2Blob->GetBufferSize() };

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_cskc2PSO)));

	//Unload compute shader
	UnloadShaders();

}

//Initialize kc3 compute shader root signature and pipeline state object
void CQRender::InitCSKC3RSAndPSO() {

	//Create root signature

	//Check root signature version support
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureVersion = {};
	rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureVersion, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		rootSignatureVersion.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Create root signature parameters
	D3D12_ROOT_PARAMETER1 rootParameter[1] = {};
	D3D12_DESCRIPTOR_RANGE1 SRVUAVDescriptorRanges[1] = {};
	SRVUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	SRVUAVDescriptorRanges[0].NumDescriptors = 1;
	SRVUAVDescriptorRanges[0].BaseShaderRegister = 0;
	SRVUAVDescriptorRanges[0].RegisterSpace = 0;
	SRVUAVDescriptorRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	SRVUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(SRVUAVDescriptorRanges);
	rootParameter[0].DescriptorTable.pDescriptorRanges = SRVUAVDescriptorRanges;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Create root signature description
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
	rootSignatureDescription.Init_1_1(_countof(rootParameter), rootParameter, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	//Create root signature
	ID3D10Blob* serializedRootSignatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, rootSignatureVersion.HighestVersion, &serializedRootSignatureBlob, &errorBlob));
	ThrowIfFailed(m_dxDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_cskc3RootSignature)));

	SafeRelease(&serializedRootSignatureBlob);
	SafeRelease(&errorBlob);

	//Create pipeline state object

	struct PSO_SUBSTREAMS {

		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE PSORootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_CS PSOComputeShader;

	} PSOSubStreams;

	//Set root signature
	PSOSubStreams.PSORootSignature = m_cskc3RootSignature.Get();

	//Load compute shader
	LoadShaders();

	//Set compute shader
	PSOSubStreams.PSOComputeShader = { m_cskc3Blob->GetBufferPointer(), m_cskc3Blob->GetBufferSize() };

	//Create PSO
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &PSOSubStreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS);
	ThrowIfFailed(m_dxDevice->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_cskc3PSO)));

	//Unload compute shader
	UnloadShaders();

}

//Initialize kc1 compute shader centroid buffer
void CQRender::InitCentroidBuffer() {

	//Initial centroid position/color

	FLOAT randomNumber = 0.0f;
	randomNumber = 1.0f / static_cast<FLOAT>(COMPUTE_SHADER_KC_CENTROID_COUNT);
	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		if (COMPUTE_SHADER_KC_CENTROID_INIT_FULL_RANDOM) {

			randomNumber = static_cast<FLOAT>(std::rand()) / RAND_MAX;
			m_clearCentroidBuffer[i].color[0] = randomNumber;
			m_clearCentroidBuffer[i].color[1] = randomNumber;
			m_clearCentroidBuffer[i].color[2] = randomNumber;

		}
		else if (COMPUTE_SHADER_KC_CENTROID_INIT_NO_RANDOM) {

			m_clearCentroidBuffer[i].color[0] = randomNumber * static_cast<FLOAT>(i);
			m_clearCentroidBuffer[i].color[1] = randomNumber * static_cast<FLOAT>(i);
			m_clearCentroidBuffer[i].color[2] = randomNumber * static_cast<FLOAT>(i);
		
		}
		else if (COMPUTE_SHADER_KC_CENTROID_INIT_FROM_PIXEL_SET) {

			m_clearCentroidBuffer[i].color[0] = m_initialCentroidColors[i][0];
			m_clearCentroidBuffer[i].color[1] = m_initialCentroidColors[i][1];
			m_clearCentroidBuffer[i].color[2] = m_initialCentroidColors[i][2];

		}
		
	}

	//Copy into copy buffer
	BYTE* centroidCopyBufferBits = nullptr;
	SIZE_T copySize = SIZE_T(COMPUTE_SHADER_KC_CENTROID_COUNT * sizeof(Centroid));
	D3D12_RANGE writeRange = {0, copySize };
	D3D12_RANGE readRange = {0, 0};
	ThrowIfFailed(m_cskc1CentroidCopyBuffer->Map(0, &readRange, reinterpret_cast<void**>(&centroidCopyBufferBits)));
	::memcpy(centroidCopyBufferBits, &m_clearCentroidBuffer[0], copySize);
	m_cskc1CentroidCopyBuffer->Unmap(0, &writeRange);

	//Copy into centroid buffer
	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();
	
	//Set centroid buffer state to D3D12_RESOURCE_STATE_COPY_DEST
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_cskc1CentroidBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	commandList->CopyBufferRegion(m_cskc1CentroidBuffer.Get(), 0, m_cskc1CentroidCopyBuffer.Get(), 0, static_cast<UINT64>(copySize));

	//Set centroid buffer state to D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_cskc1CentroidBuffer.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));

}

//Readback kc1 compute shader centroid buffer
void CQRender::ReadbackCentroidBuffer() {

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();

	SIZE_T bufferSize = COMPUTE_SHADER_KC_CENTROID_COUNT * sizeof(Centroid);
	commandList->CopyBufferRegion(m_cskc1CentroidReadbackBuffer.Get(), 0, m_cskc1CentroidBuffer.Get(), 0, UINT64(bufferSize));
	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

	BYTE* centroidReadbackBufferBits = nullptr;
	D3D12_RANGE readRange = {0, bufferSize};
	D3D12_RANGE writeRange = {0, 0};
	ThrowIfFailed(m_cskc1CentroidReadbackBuffer->Map(0, &readRange, reinterpret_cast<void**>(&centroidReadbackBufferBits)));

	::memcpy(m_readbackCentroidBuffer, centroidReadbackBufferBits, bufferSize);

	m_cskc1CentroidReadbackBuffer->Unmap(0, &writeRange);

}

//Initialize kc1 compute shader texture buffer
void CQRender::InitTextureBuffer() {

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();

	//Set texture buffer state to D3D12_RESOURCE_STATE_COPY_DEST
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_cskc1PictureTexture.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	//Copy into texture buffer
	D3D12_TEXTURE_COPY_LOCATION destinationCopyLocation = {};
	D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation = {};

	destinationCopyLocation.pResource = m_cskc1PictureTexture.Get();
	destinationCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destinationCopyLocation.SubresourceIndex = 0;

	sourceCopyLocation.pResource = m_sampleCopyBackBuffer.Get();
	sourceCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	sourceCopyLocation.PlacedFootprint = m_cskc1PictureTextureFootprint;

	commandList->CopyTextureRegion(&destinationCopyLocation, 0, 0, 0, &sourceCopyLocation, NULL);

	//Set texture buffer state to D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_cskc1PictureTexture.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));

}

//Initialize kc1 compute shader centroid colors
void CQRender::InitCentroidColors(IMFSample* sample) {

	IMFMediaBuffer* mediaBuffer = nullptr;
	BYTE* mediaBufferBits = nullptr;

	DWORD mediaBufferMaxLength = 0;
	DWORD mediaBufferCurrentLength = 0;

	ThrowIfFailed(sample->GetBufferByIndex(0, &mediaBuffer));
	ThrowIfFailed(mediaBuffer->Lock(&mediaBufferBits, &mediaBufferMaxLength, &mediaBufferCurrentLength));

	DWORD pixelCount = mediaBufferCurrentLength / 4;
	DWORD* mediaBufferBitsDWORD = (DWORD*)mediaBufferBits;
	DWORD randomNumberColors = 0;
	UINT randomNumber = 0;
	UINT8 uintColorR = 0;
	UINT8 uintColorG = 0;
	UINT8 uintColorB = 0;
	FLOAT floatColorR = 0.0f;
	FLOAT floatColorG = 0.0f;
	FLOAT floatColorB = 0.0f;

	for (UINT i = 0; i < COMPUTE_SHADER_KC_CENTROID_COUNT; ++i) {

		randomNumber = std::rand() % pixelCount;
		randomNumberColors = *(mediaBufferBitsDWORD + randomNumber);

		uintColorR = static_cast<UINT8>((randomNumberColors & 0b00000000111111110000000000000000) >> 16);
		uintColorG = static_cast<UINT8>((randomNumberColors & 0b00000000000000001111111100000000) >> 8);
		uintColorB = static_cast<UINT8>(randomNumberColors & 0b00000000000000000000000011111111);

		floatColorR = static_cast<FLOAT>(uintColorR) / 255.0f;
		floatColorG = static_cast<FLOAT>(uintColorG) / 255.0f;
		floatColorB = static_cast<FLOAT>(uintColorB) / 255.0f;

		m_initialCentroidColors[i][0] = floatColorR;
		m_initialCentroidColors[i][1] = floatColorG;
		m_initialCentroidColors[i][2] = floatColorB;

	}

	ThrowIfFailed(mediaBuffer->Unlock());
	SafeRelease(&mediaBuffer);

}

//Upload IMFSample IMFMediaBuffer data into GPU
void CQRender::UploadSampleToGPU(IMFSample* sample, ID3D12Resource* intermediateBuffer, ID3D12Resource* destinationBuffer) {

	IMFMediaBuffer* mediaBuffer = nullptr;
	BYTE* mediaBufferBits = nullptr;
	BYTE* intermediateBufferBits = nullptr;
	DWORD maxBufferLength = 0;
	DWORD currentBufferLength = 0;

	//Get media buffer from the sample
	ThrowIfFailed(sample->GetBufferByIndex(0, &mediaBuffer));
	
	//Get pointer to the media buffer data
	ThrowIfFailed(mediaBuffer->Lock(&mediaBufferBits, &maxBufferLength, &currentBufferLength));
	
	//Get pointer to the ID3D12Resource intermediate buffer data
	D3D12_RANGE readRange = {0, 0};
	D3D12_RANGE writeRange = { 0, static_cast<SIZE_T>(currentBufferLength)};
	ThrowIfFailed(intermediateBuffer->Map(0, &readRange, reinterpret_cast<void**>(&intermediateBufferBits)));

	//Copy into intermediate buffer
	::memcpy(intermediateBufferBits, mediaBufferBits, static_cast<SIZE_T>(currentBufferLength));

	//Release pointers to buffer data
	intermediateBuffer->Unmap(0, &writeRange);
	ThrowIfFailed(mediaBuffer->Unlock());

	//Release media buffer COM object
	SafeRelease(&mediaBuffer);

	//Get command list from copy command queue
	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_copyCQ->GetCommandList();
	
	//Add copy instruction to command list
	commandList->CopyBufferRegion(destinationBuffer, 0, intermediateBuffer, 0, static_cast<UINT64>(currentBufferLength));

	//Execute command list
	m_copyCQ->WaitForFenceValue(m_copyCQ->ExecuteCommandList(commandList));

}

//Fill copy buffer for back buffer from IMFSample IMFMediaBuffer
void CQRender::LoadCopyBuffer(IMFSample* sample, ID3D12Resource* intermediateBuffer) {

	IMFMediaBuffer* mediaBuffer = nullptr;
	ThrowIfFailed(sample->GetBufferByIndex(0, &mediaBuffer));

	BYTE* mediaBufferBits = nullptr;
	BYTE* intermediateBufferBits = nullptr;
	DWORD mediaBufferMaxLength = 0;
	DWORD mediaBufferCurrentLength = 0;

	D3D12_RANGE readRange = { 0, 0 };
	D3D12_RANGE writeRange = { 0, static_cast<SIZE_T>(mediaBufferCurrentLength) };

	ThrowIfFailed(mediaBuffer->Lock(&mediaBufferBits, &mediaBufferMaxLength, &mediaBufferCurrentLength));
	ThrowIfFailed(intermediateBuffer->Map(0, &readRange, reinterpret_cast<void**>(&intermediateBufferBits)));

	::memcpy(intermediateBufferBits, mediaBufferBits, static_cast<SIZE_T>(mediaBufferCurrentLength));

	intermediateBuffer->Unmap(0, &writeRange);
	ThrowIfFailed(mediaBuffer->Unlock());
	SafeRelease(&mediaBuffer);

}

//Make unit vector from x and y mouse screen coordinates
DirectX::XMVECTOR CQRender::GetUnitVector(WORD mX, WORD mY) {

	FLOAT vectorX = 0.0f;
	FLOAT vectorY = 0.0f;
	FLOAT vectorZ = 0.0f;
	FLOAT xySquared = 0.0f;

	auto sphereDiameter = min(WINDOW_WIDTH, WINDOW_HEIGHT);
	FLOAT sphereRadius = static_cast<FLOAT>(sphereDiameter) / 2.0f;

	//Not normalized coordinates
	vectorX = (FLOAT) mX - (FLOAT)(WINDOW_WIDTH / 2);
	vectorY = (FLOAT)(WINDOW_HEIGHT - mY) - (FLOAT)(WINDOW_HEIGHT / 2);

	//Cap coordinates
	if (abs(vectorX) > sphereRadius) {
		
		if (vectorX < 0.0f) vectorX = -sphereRadius;
		else vectorX = sphereRadius;
	
	}

	if (abs(vectorY) > sphereRadius) {

		if (vectorY < 0.0f) vectorY = -sphereRadius;
		else vectorY = sphereRadius;

	}

	//Normalized coordinates
	vectorX = vectorX / sphereRadius;
	vectorY = vectorY / sphereRadius;

	//Get Z coordinate
	xySquared = vectorX * vectorX + vectorY * vectorY;
	if (xySquared > 1.0f) {
		
		DirectX::XMVECTOR returnVector = DirectX::XMVector3Normalize(DirectX::XMVectorSet(vectorX, vectorY, 0.0f, 0.0f));
		return returnVector;

	}
	
	vectorZ = -sqrtf(1.0f - xySquared);
	return DirectX::XMVectorSet(vectorX, vectorY, vectorZ, 0.0f);

}

//K-means clustering (1) compute shader
void CQRender::CSKC1() {

	//mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();

	ID3D12DescriptorHeap* descriptorHeaps[1] = { m_cskc1SRVDescriptorHeap.Get() };
	
	UINT descriptorHandleIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_GPU_DESCRIPTOR_HANDLE cskc1FirstDescriptorHandle = m_cskc1SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE cskc3FirstDescriptorHandle = m_cskc1SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	cskc3FirstDescriptorHandle.ptr = SIZE_T(UINT64(cskc3FirstDescriptorHandle.ptr) + UINT64(descriptorHandleIncrementSize));


	for (UINT i = 0; i < COMPUTE_SHADER_KC_LOOP_COUNT; ++i) {

		mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();

		//CSKC1
		commandList->SetPipelineState(m_cskc1PSO.Get());
		commandList->SetComputeRootSignature(m_cskc1RootSignature.Get());

		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandList->SetComputeRootDescriptorTable(0, cskc1FirstDescriptorHandle);

		commandList->Dispatch(1280 / 8, 720 / 8, 1); //should be rounding up the division
		
		m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));
		commandList = nullptr;
		commandList = m_directCQ->GetCommandList();

		ReadbackCentroidBuffer();

		UINT64 pixelCount = 0;
		for (UINT j = 0; j < COMPUTE_SHADER_KC_CENTROID_COUNT; ++j) pixelCount += m_readbackCentroidBuffer[j].count;

		FLOAT uintSumToFloat[3] = { 
		
			static_cast<FLOAT>(m_readbackCentroidBuffer[0].uintSum[0]) / 255.0f,
			static_cast<FLOAT>(m_readbackCentroidBuffer[0].uintSum[1]) / 255.0f,
			static_cast<FLOAT>(m_readbackCentroidBuffer[0].uintSum[2]) / 255.0f,

		};

		//CSKC3
		commandList->SetPipelineState(m_cskc3PSO.Get());
		commandList->SetComputeRootSignature(m_cskc3RootSignature.Get());

		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandList->SetComputeRootDescriptorTable(0, cskc3FirstDescriptorHandle);

		commandList->Dispatch(COMPUTE_SHADER_KC_CENTROID_COUNT, 1, 1);

		m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));

		ReadbackCentroidBuffer();

	}

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();

	//Set texture buffer state to D3D12_RESOURCE_STATE_UNORDERED_ACCESS 
	{
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_cskc1PictureTexture.Get();
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		commandList->ResourceBarrier(1, &resourceBarrier);
	}

	m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));

	ReadbackCentroidBuffer();

}

//K-means clustering (2) compute shader
void CQRender::CSKC2() {

	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();

	commandList->SetPipelineState(m_cskc2PSO.Get());
	commandList->SetComputeRootSignature(m_cskc2RootSignature.Get());

	D3D12_GPU_DESCRIPTOR_HANDLE firstDescriptorHandle = m_cskc1SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorHandleIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	firstDescriptorHandle.ptr = SIZE_T(UINT64(firstDescriptorHandle.ptr) + UINT64(descriptorHandleIncrementSize));
	ID3D12DescriptorHeap* descriptorHeaps[1] = { m_cskc1SRVDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	commandList->SetComputeRootDescriptorTable(0, firstDescriptorHandle);

	commandList->Dispatch(1280 / 8, 720 / 8, 1); //should be rounding up the division

	m_directCQ->WaitForFenceValue(m_directCQ->ExecuteCommandList(commandList));

}