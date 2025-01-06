#include <cube_lighting_render.h>

#include <d3dx12.h>

//Link DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "D3DCompiler.lib")

//DirectX includes
#include <d3d12.h>
#include <d3dcompiler.h>
#include <directxmath.h>

//Project external includes
#include <ctime>

//Project internal includes
#include <helper.h>
#include <config.h>
#include <commandqueue.h>
#include <resource.h>

namespace mWRL = Microsoft::WRL;

void CubeLightingRender::LoadContent(double* updateFPS, D3D12_RESOURCE_DESC backBufferDescription) {
	
	*updateFPS = 0.0001;

	this->LoadShaders();
	this->InitVertexBufferTriangleListUnitCube();
	this->InitRootSignature();
	this->InitPipelineStateObject();
	this->InitDepthBuffer();

	m_isLoaded = TRUE;

}

BOOL CubeLightingRender::OnRender(mWRL::ComPtr<ID3D12GraphicsCommandList> commandList, mWRL::ComPtr<ID3D12Resource> backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE backBufferDescriptor) {

	if (!m_isLoaded) {

		return FALSE;

	}

	//Get depth buffer descriptor
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

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
	FLOAT backBufferClearValue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_RECT clearRectangle = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	commandList->ClearRenderTargetView(backBufferDescriptor, backBufferClearValue, 1, &clearRectangle);				//Back buffer clear
	commandList->ClearDepthStencilView(depthStencilDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);			//Depth stencil buffer clear

	//Set pipeline state object and root signature
	commandList->SetPipelineState(m_pipelineStateObject->GetPipelineStateObject().Get());
	commandList->SetGraphicsRootSignature(m_rootSignature->GetRootSignature().Get());

	//Set primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set vertex buffer
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferViewTriangleListUnitCube);

	//Set root signature parameters
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &m_worldMatrix, 0);
	commandList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMMATRIX) / 4, &m_viewMatrix, 0);
	commandList->SetGraphicsRoot32BitConstants(2, sizeof(DirectX::XMMATRIX) / 4, &m_projectionMatrix, 0);
	commandList->SetGraphicsRoot32BitConstants(3, sizeof(DirectX::XMVECTOR) / 4, &m_cameraPosition, 0);

	//Set scissor rectangle and viewport
	commandList->RSSetViewports(1, &m_viewport);
	commandList->RSSetScissorRects(1, &m_scissorRectangle);

	//Set render target
	commandList->OMSetRenderTargets(1, &backBufferDescriptor, NULL, &depthStencilDescriptor);

	//Draw
	commandList->DrawInstanced(36, 1, 0, 0);

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

BOOL CubeLightingRender::OnUpdate(double elapsedTime, BYTE flag) {

	if (!m_isLoaded) {

		return FALSE;

	}

	return TRUE;

}

void CubeLightingRender::OnMouseWheel(float mouseWheelData) {

	DirectX::XMMATRIX scalingMatrix = {};
	if (mouseWheelData < 0.0f) {

		scalingMatrix = DirectX::XMMatrixScaling(0.9f, 0.9f, 0.9f);

	}
	else {

		scalingMatrix = DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f);

	}

	m_cameraPosition = DirectX::XMVector4Transform(m_cameraPosition, scalingMatrix);
	const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, focusPosition, m_upDirection);

}

void CubeLightingRender::OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2) {

	//Get unit vectors
	DirectX::XMVECTOR v1 = this->GetUnitVector(mX1, mY1);
	DirectX::XMVECTOR v2 = this->GetUnitVector(mX2, mY2);

	//Get rotation angle
	DirectX::XMVECTOR rotationAngleVector = DirectX::XMVector3Dot(v1, v2);
	FLOAT rotationAngle = DirectX::XMVectorGetX(rotationAngleVector);
	rotationAngle = min(1.0f, acosf(rotationAngle)) * 2.0f;

	//Get rotation axis
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(v1, v2);

	//Get rotation transform
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, rotationAngle);
	
	//Rotate the camera
	{

		/*m_cameraPosition = DirectX::XMVector4Transform(m_cameraPosition, rotationMatrix);
		m_upDirection = DirectX::XMVector4Transform(m_upDirection, rotationMatrix);

		const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, focusPosition, m_upDirection);*/
	
	}

	//Rotate the object
	m_worldMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, rotationMatrix);

}

void CubeLightingRender::OnSpace() {



}

CubeLightingRender::CubeLightingRender(Microsoft::WRL::ComPtr<ID3D12Device2> dxDevice, std::shared_ptr<CommandQueue> copyCQ, std::shared_ptr<CommandQueue> directCQ)
	: m_device(dxDevice), m_copyCQ(copyCQ), m_directCQ(directCQ) {

	//Intialize vertex and index buffer views
	m_vertexBufferViewTriangleListUnitCube = { 0 };

	//Initialize world, view and projection matrices
	//m_worldMatrix = DirectX::XMMatrixTranslation(-0.5f, -0.5f, -0.5f);
	m_worldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	/*DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0.785f, 0.785f, 0.0f);
	m_worldMatrix = DirectX::XMMatrixMultiply(m_worldMatrix, rotation);*/

	m_cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);
	const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, focusPosition, m_upDirection);

	const float aspectRatio = ((float)WINDOW_WIDTH) / ((float)WINDOW_HEIGHT);
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), aspectRatio, 0.1f, 100.0f);

	//Check for root signature version support
	m_rootSignatureVersionSupport.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &m_rootSignatureVersionSupport, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) {
		m_rootSignatureVersionSupport.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	//Set viewport and scissor rectangle
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(WINDOW_WIDTH);
	m_viewport.Height = static_cast<FLOAT>(WINDOW_HEIGHT);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_scissorRectangle.left = 0;
	m_scissorRectangle.top = 0;
	m_scissorRectangle.right = LONG_MAX;
	m_scissorRectangle.bottom = LONG_MAX;

}

CubeLightingRender::~CubeLightingRender() {



}



//Load shaders
void CubeLightingRender::LoadShaders() {

	ThrowIfFailed(D3DReadFileToBlob(L"./cube_lighting_render_vertex.cso", &m_vertexShader)); //Vertex shader
	ThrowIfFailed(D3DReadFileToBlob(L"./cube_lighting_render_pixel.cso", &m_pixelShader)); //Vertex shader

}

//Initialize vertex buffer - triangle list - unit cube
void CubeLightingRender::InitVertexBufferTriangleListUnitCube() {

	//Vertex buffer size -- 12 triangles * 9(3 * points + 3 * color + 3 * normal) * 3(coordinates) * 4(FLOAT) 
	UINT64 vertexBufferSizeInBytes = 1296;
	BYTE* uploadBufferPtr = nullptr;
	D3D12_RANGE uploadBufferWriteRange = { 0, (SIZE_T)vertexBufferSizeInBytes };

	m_vertexBufferTriangleListUnitCube = std::make_unique<Resource>(m_device, m_copyCQ, RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_DIMENSION_BUFFER, vertexBufferSizeInBytes, 1,
		D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

	m_vertexBufferTriangleListUnitCube->MapUploadBufferPtr(0, reinterpret_cast<void**>(&uploadBufferPtr));

	BYTE* pointAPtr = nullptr;
	BYTE* pointBPtr = nullptr;
	BYTE* pointCPtr = nullptr;
	BYTE* pointColorPtr = nullptr;
	BYTE* pointNormalPtr = nullptr;

	Point normal = {};
	pointNormalPtr = reinterpret_cast<BYTE*>(&normal.x);

	Point color(160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f);
	pointColorPtr = reinterpret_cast<BYTE*>(&color.x);

	//Face 1.
	normal = Point(0.0f, 0.0f, -1.0f);
	
	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[0].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[1].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[2].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[0].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[2].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[3].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	//Face 2.
	normal = Point(0.0f, 0.0f, 1.0f);

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[4].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[5].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[6].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[4].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[6].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[7].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	//Face 3.
	normal = Point(-1.0f, 0.0f, 0.0f);

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[4].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[5].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[1].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[4].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[1].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[0].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	//Face 4.
	normal = Point(1.0f, 0.0f, 0.0f);

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[7].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[6].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[2].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[7].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[2].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[3].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	//Face 5.
	normal = Point(0.0f, -1.0f, 0.0f);

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[0].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[4].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[7].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[0].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[7].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[3].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	//Face 6.
	normal = Point(0.0f, 1.0f, 0.0f);

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[1].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[5].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[6].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;

	pointAPtr = reinterpret_cast<BYTE*>(&m_vertices[1].x); pointBPtr = reinterpret_cast<BYTE*>(&m_vertices[6].x); pointCPtr = reinterpret_cast<BYTE*>(&m_vertices[2].x);
	::memcpy(uploadBufferPtr, pointAPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointBPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;
	::memcpy(uploadBufferPtr, pointCPtr, 12); ::memcpy(uploadBufferPtr + 12, pointColorPtr, 12); ::memcpy(uploadBufferPtr + 24, pointNormalPtr, 12);
	uploadBufferPtr += 36;


	m_vertexBufferTriangleListUnitCube->UnmapUploadBufferPtr(0, &uploadBufferWriteRange);

	m_vertexBufferTriangleListUnitCube->CopyUploadToDefault();

	m_vertexBufferViewTriangleListUnitCube.BufferLocation = m_vertexBufferTriangleListUnitCube->GetDefaultGPUVirtualAddress();
	m_vertexBufferViewTriangleListUnitCube.SizeInBytes = static_cast<UINT>(vertexBufferSizeInBytes);
	m_vertexBufferViewTriangleListUnitCube.StrideInBytes = 36; // 3(point + color + normal) * 3(coordinates) * 4(FLOAT)

}

//Initialize root signature
void CubeLightingRender::InitRootSignature() {

	D3D12_ROOT_PARAMETER1 rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[0].Constants.RegisterSpace = 0;
	rootParameters[0].Constants.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[1].Constants.RegisterSpace = 0;
	rootParameters[1].Constants.ShaderRegister = 1;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].Constants.Num32BitValues = sizeof(DirectX::XMMATRIX) / 4;
	rootParameters[2].Constants.RegisterSpace = 0;
	rootParameters[2].Constants.ShaderRegister = 2;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Constants.Num32BitValues = sizeof(DirectX::XMVECTOR) / 4;
	rootParameters[3].Constants.RegisterSpace = 0;
	rootParameters[3].Constants.ShaderRegister = 0;

	m_rootSignature = std::make_unique<RootSignature>(m_device, &m_rootSignatureVersionSupport,
		rootParameters, static_cast<UINT>(_countof(rootParameters)), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

}

//Intialize pipeline state object - rasterizer set to fill mode, no culling
void CubeLightingRender::InitPipelineStateObject() {

	//Input assembler layout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescription = {};
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementsDescription[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	};
	inputLayoutDescription.pInputElementDescs = inputLayoutElementsDescription;
	inputLayoutDescription.NumElements = _countof(inputLayoutElementsDescription);

	//Vertex shader
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = m_vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = m_vertexShader->GetBufferPointer();

	//Pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = m_pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = m_pixelShader->GetBufferPointer();

	//Render target formats
	D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
	renderTargetFormats.RTFormats[0] = SWAP_CHAIN_BUFFER_FORMAT;
	renderTargetFormats.NumRenderTargets = 1;

	//Rasterizer
	CD3DX12_RASTERIZER_DESC rasterizerDescription = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE,
		FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	//Blending
	D3D12_BLEND_DESC blendDescription = {};
	blendDescription.AlphaToCoverageEnable = FALSE;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.RenderTarget[0].BlendEnable = FALSE;
	blendDescription.RenderTarget[0].LogicOpEnable = FALSE;
	blendDescription.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	CD3DX12_BLEND_DESC cd3dx12_blendDescription = CD3DX12_BLEND_DESC(blendDescription);

	m_pipelineStateObject = std::make_unique<PipelineState>(m_device, &inputLayoutDescription, m_rootSignature->GetRootSignature(), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		&vertexShaderBytecode, &pixelShaderBytecode, DXGI_FORMAT_D32_FLOAT, &renderTargetFormats, &rasterizerDescription, &cd3dx12_blendDescription);

}

//Initialize depth buffer
void CubeLightingRender::InitDepthBuffer() {

	//Create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescription = {};
	descriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDescription.NodeMask = 0;
	descriptorHeapDescription.NumDescriptors = 1;
	
	ThrowIfFailed(m_device->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(&m_dsvDescriptorHeap)));

	//Create depth buffer
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	m_depthBuffer = std::make_unique<Resource>(m_device, m_copyCQ, RESOURCE_NO_UPLOAD | RESOURCE_NO_READBACK, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		WINDOW_WIDTH, WINDOW_HEIGHT, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 1, 1, CUBELIGHTINGRENDER_DEPTH_STENCIL_BUFFER_FORMAT, 1, 0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clearValue);

	//Create depth stencil view
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDescription = {};
	dsvDescription.Format = CUBELIGHTINGRENDER_DEPTH_STENCIL_BUFFER_FORMAT;
	dsvDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDescription.Texture2D.MipSlice = 0;
	dsvDescription.Flags = D3D12_DSV_FLAG_NONE;

	m_device->CreateDepthStencilView(m_depthBuffer->GetDefaultBuffer().Get(), &dsvDescription, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

//Make unit vector from x and y mouse screen coordinates
DirectX::XMVECTOR CubeLightingRender::GetUnitVector(WORD mX, WORD mY) {

	FLOAT vectorX = 0.0f;
	FLOAT vectorY = 0.0f;
	FLOAT vectorZ = 0.0f;
	FLOAT xySquared = 0.0f;

	auto sphereDiameter = min(WINDOW_WIDTH, WINDOW_HEIGHT);
	FLOAT sphereRadius = static_cast<FLOAT>(sphereDiameter) / 2.0f;

	//Not normalized coordinates
	vectorX = (FLOAT)mX - (FLOAT)(WINDOW_WIDTH / 2);
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

