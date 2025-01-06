#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <memory>

#include <IRender.h>
#include <resource.h>
#include <rootsignature.h>
#include <pipelinestate.h>
#include <commandqueue.h>

class CubeLightingRender : public IRender {

public:

	void LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription);
	BOOL OnRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_CPU_DESCRIPTOR_HANDLE);
	BOOL OnUpdate(double elapsedTime, BYTE flag);
	void OnMouseWheel(float mouseWheelData);
	void OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2);
	void OnSpace();
	CubeLightingRender(Microsoft::WRL::ComPtr<ID3D12Device2> dxDevice, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ);
	~CubeLightingRender();

private:

	void InitVertexBufferTriangleListUnitCube();
	void InitRootSignature();
	void InitPipelineStateObject();
	void InitDepthBuffer();
	void LoadShaders();

	DirectX::XMVECTOR GetUnitVector(WORD mX, WORD mY);

	struct Point {

	public:

		FLOAT x;
		FLOAT y;
		FLOAT z;

		Point(FLOAT x, FLOAT y, FLOAT z) : x(x), y(y), z(z) {}
		Point() : x(0.0f), y(0.0f), z(0.0f) {}
		Point(const Point& point) {

			x = point.x;
			y = point.y;
			z = point.z;

		}
		Point& operator=(const Point& point) {

			x = point.x;
			y = point.y;
			z = point.z;

			return *this;

		}

	};


	Point m_vertices[8] = {
	
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f},
	
	};

	BOOL m_isLoaded = FALSE;

	std::shared_ptr<CommandQueue> m_copyCQ;
	std::shared_ptr<CommandQueue> m_directCQ;

	Microsoft::WRL::ComPtr<ID3D12Device2> m_device;

	std::unique_ptr<Resource> m_vertexBufferTriangleListUnitCube;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViewTriangleListUnitCube;

	std::unique_ptr<Resource> m_depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

	D3D12_FEATURE_DATA_ROOT_SIGNATURE m_rootSignatureVersionSupport;

	std::unique_ptr<RootSignature> m_rootSignature;
	std::unique_ptr<PipelineState> m_pipelineStateObject;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelShader;

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_worldMatrix;

	DirectX::XMVECTOR m_cameraPosition;
	DirectX::XMVECTOR m_upDirection;

	D3D12_RECT m_scissorRectangle;
	D3D12_VIEWPORT m_viewport;

};