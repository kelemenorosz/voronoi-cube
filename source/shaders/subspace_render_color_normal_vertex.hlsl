
cbuffer WorldMatrix : register(b0) {

	matrix worldMatrix;

};

cbuffer ViewMatrix : register(b1) {

	matrix viewMatrix;

};

cbuffer ProjectionMatrix : register(b2) {

	matrix projectionMatrix;

};

struct VertexShaderInput {

	float3 Position : POSITION;
	float3 Color : COLOR;
	float3 Normal : NORMAL;

};

struct VertexShaderOutput {

	float3 Color : COLOR;
	float3 FragmentPosition : FPOSITION;
	float3 Normal : NORMAL;
	float4 Position : SV_Position;

};

VertexShaderOutput main(VertexShaderInput IN)
{

	VertexShaderOutput OUT;

	static matrix<float, 4, 4> Identity = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	static matrix<float, 4, 4> Translation = {
	1.0f, 0.0f, 0.0f, -0.5f,
	0.0f, 1.0f, 0.0f, -0.5f,
	0.0f, 0.0f, 1.0f, -0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	OUT.Position = mul(Translation, float4(IN.Position, 1.0f));
	OUT.Position = mul(worldMatrix, OUT.Position);
	OUT.Position = mul(viewMatrix, OUT.Position);
	OUT.Position = mul(projectionMatrix, OUT.Position);

	float4 RotatedFragment = mul(Translation, float4(IN.Position, 1.0f));
	RotatedFragment = mul(worldMatrix, RotatedFragment);
	OUT.FragmentPosition = RotatedFragment.xyz;
	
	float4 RotatedNormal = mul(worldMatrix, float4(IN.Normal, 1.0f));
	OUT.Normal = RotatedNormal.xyz;

	OUT.Color = IN.Color;

	return OUT;

}