#include <pipelinestate.h>

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

PipelineState::PipelineState(mWRL::ComPtr<ID3D12Device2> device, D3D12_INPUT_LAYOUT_DESC* inputLayout, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primiviteTopology, D3D12_SHADER_BYTECODE* vertexShader, D3D12_SHADER_BYTECODE* pixelShader,
	DXGI_FORMAT depthStencilFormat, D3D12_RT_FORMAT_ARRAY* renderTargetFormats, CD3DX12_RASTERIZER_DESC* rasterizer, CD3DX12_BLEND_DESC* blendDescription) {

	PSO_SUBSTREAMS_GRAPHICS psoSubstreams = {};
	psoSubstreams.PSOInputLayout = *inputLayout;
	psoSubstreams.PSORootSignature = rootSignature.Get();
	psoSubstreams.PSOPrimitiveTopology = primiviteTopology;
	psoSubstreams.PSOVertexShader = *vertexShader;
	psoSubstreams.PSOPixelShader = *pixelShader;
	psoSubstreams.PSODepthStencilFormat = depthStencilFormat;
	psoSubstreams.PSORenderTargetFormats = *renderTargetFormats;
	psoSubstreams.PSORasterizer = *rasterizer;
	psoSubstreams.PSOBlendDescription = *blendDescription;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &psoSubstreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS_GRAPHICS);
	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_pipelineStateObject)));

}

PipelineState::PipelineState(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, D3D12_SHADER_BYTECODE* computeShader) {

	PSO_SUBSTREAMS_COMPUTE psoSubstreams = {};
	psoSubstreams.PSORootSignature = rootSignature.Get();
	psoSubstreams.PSOComputeShader = *computeShader;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDescription = {};
	pipelineStateStreamDescription.pPipelineStateSubobjectStream = &psoSubstreams;
	pipelineStateStreamDescription.SizeInBytes = sizeof(PSO_SUBSTREAMS_COMPUTE);
	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDescription, IID_PPV_ARGS(&m_pipelineStateObject)));

}

PipelineState::~PipelineState() {



}

mWRL::ComPtr<ID3D12PipelineState> PipelineState::GetPipelineStateObject() {

	return m_pipelineStateObject;

}