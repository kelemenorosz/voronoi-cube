#pragma once

#include <d3d12.h>
#include <directxmath.h>

#include <wrl.h>
#include <memory>

#include <IRender.h>
#include <commandqueue.h>
#include <resolver.h>
#include <config.h>

class CQRender : public IRender {


public:

	CQRender(Microsoft::WRL::ComPtr<ID3D12Device2> dxDevice, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ);
	~CQRender();
	void LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription);
	BOOL OnRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE backBufferDescriptor);
	BOOL OnUpdate(double elapsedTime, BYTE flag);
	void OnMouseWheel(float mouseWheelData);
	void OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2);
	void OnSpace();

private:

	DirectX::XMFLOAT4 m_pixelData[8] = {
	
		{-1.0f, -1.0f, -1.0f, 1.0f},	//0
		{-1.0f, 1.0f, -1.0f, 1.0f},		//1
		{1.0f, 1.0f, -1.0f, 1.0f},		//2
		{1.0f, -1.0f, -1.0f, 1.0f},		//3
		{-1.0f, -1.0f, 1.0f, 1.0f},		//4
		{-1.0f, 1.0f, 1.0f, 1.0f},		//5
		{1.0f, 1.0f, 1.0f, 1.0f},		//6
		{1.0f, -1.0f, 1.0f, 1.0f},		//7

	};

	DirectX::XMFLOAT4 m_colorData[8] = {

		{0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},


	};

	//WORD m_indexData[24] = {

	//	0, 1,
	//	0, 3,
	//	2, 1,
	//	2, 3,
	//	4, 5,
	//	4, 7,
	//	6, 5,
	//	6, 7,
	//	0, 4,
	//	1, 5,
	//	2, 6,
	//	3, 7

	//};

	WORD m_indexData[6] = {

		0, 1,
		0, 3,
		0, 4,

	};

	/*struct Centroid {
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 sum;
		DWORD count;
	};*/

	struct Centroid {
		FLOAT color[3];
		FLOAT sum[3];
		UINT32 uintSum[3];
		DWORD count;
	};

	Centroid m_clearCentroidBuffer[COMPUTE_SHADER_KC_CENTROID_COUNT] = {};
	Centroid m_readbackCentroidBuffer[COMPUTE_SHADER_KC_CENTROID_COUNT] = {};
	FLOAT m_initialCentroidColors[COMPUTE_SHADER_KC_CENTROID_COUNT][3] = {};

	void InitPSO();
	void InitPointPSO();
	void InitCentroidCubePointRSAndPSO();
	void InitRootSignature();
	void InitDS();
	void InitIABuffer(void* buffer, UINT64 width, ID3D12Resource** destinationBuffer, void* destinationBufferView, UINT stride = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
	void LoadShaders();
	void UnloadShaders();
	void InitComputeShaderResources(UINT64 inputWidth, UINT64 inputHeight);
	void InitComputeShaderRSAndPSO();
	void InitCentroidCubePointVertexBuffer();
	void UploadSampleToGPU(IMFSample* sample, ID3D12Resource* intermediateBuffer, ID3D12Resource* destinationBuffer);
	void LoadCopyBuffer(IMFSample* sample, ID3D12Resource* intermediateBuffer);
	DirectX::XMVECTOR GetUnitVector(WORD mX, WORD mY);

	//Compute shader - k-means clustering
	void InitCSKC1Resources(UINT64 inputWidth, UINT64 inputHeight);
	void InitCSKC1RSAndPSO();
	void InitCSKC2RSAndPSO();
	void InitCSKC3RSAndPSO();
	void InitCentroidBuffer();
	void InitTextureBuffer();
	void InitCentroidColors(IMFSample* sample);
	void CSKC1();
	void CSKC2();
	void ReadbackCentroidBuffer();

	Microsoft::WRL::ComPtr<ID3D12Device2> m_dxDevice;

	std::shared_ptr<CommandQueue> m_directCQ;
	std::shared_ptr<CommandQueue> m_copyCQ;

	std::unique_ptr<Resolver> m_resolver;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pointPSO;
	
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_emptyRootSignature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_constantRootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pointVertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pointPixelShaderBlob;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_dsBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pixelVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_colorVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_pixelVBView;
	D3D12_VERTEX_BUFFER_VIEW m_colorVBView;
	D3D12_INDEX_BUFFER_VIEW m_IBView;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescriptorHeap;

	DirectX::XMMATRIX m_viewProjectionMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRectangle;

	BOOL m_isLoaded;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_sampleCopyBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_sampleVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_sampleVBView;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_sampleCopyBackBuffer;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_sampleSubresourceFootprint;

	UINT64 m_sampleResourceWidth;

	//Compute shader - downscale image by 2x
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVUAVDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_csPictureInput;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_csPictureOutput;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_csPictureInputFootprint;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_csPSO;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_csRootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_csBlob;

	//Compute shader - k-means clustering first step - calculate centroids part 1

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cskc1SRVDescriptorHeap;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> m_cskc1PictureTexture;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_cskc1CentroidBuffer;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_cskc1PictureTextureFootprint;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_cskc1CentroidCopyBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_cskc1CentroidReadbackBuffer;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_cskc1PSO;
	
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_cskc1RootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_cskc1Blob;

	//Compute shader - k-means clustering second step - assign color

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_cskc2PSO;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_cskc2RootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_cskc2Blob;

	//Compute shader - k-means clustering third step - calculate centroids part 2

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_cskc3PSO;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_cskc3RootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_cskc3Blob;

	//Quantized image centroid visualization
	
	D3D12_VIEWPORT m_cubePointViewport;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_cubePointCopyBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_cubePointVertexBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_cubePointVBView;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_cubePointPSO;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_cubePointRootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> m_cubePointVertexBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_cubePointPixelBlob;

};