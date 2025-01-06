
cbuffer WorldViewProjectionMatrix : register(b0) {

	matrix worldViewProjectionMatrix;

};

struct VertexShaderInput {

	float3 Position : POSITION;
	
};

struct VertexShaderOutput {

	float4 Color : COLOR;
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

	matrix<float, 4, 4> ModelMatrix = mul(Translation, Identity);
	ModelMatrix = mul(worldViewProjectionMatrix, ModelMatrix);

	OUT.Position = mul(ModelMatrix, float4(IN.Position, 1.0f));
	OUT.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);

	return OUT;

}