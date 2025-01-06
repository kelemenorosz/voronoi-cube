
cbuffer WorldViewProjectionMatrix : register(b0) {

	matrix<float, 4, 4> worldViewProjectionMatrix;

}

struct VertexShaderInput {

	float4 Position : POSITION;

};

struct VertexShaderOutput {

	float3 Color : COLOR;
	float4 Position : SV_Position;

};

static matrix<float, 4, 4> Translation = {
	1.0f, 0.0f, 0.0f, -0.5f,
	0.0f, 1.0f, 0.0f, -0.5f,
	0.0f, 0.0f, 1.0f, -0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
};

VertexShaderOutput main(VertexShaderInput IN)
{

	VertexShaderOutput OUT;

	float ColorR = IN.Position.x;
	float ColorG = IN.Position.y;
	float ColorB = IN.Position.z;

	OUT.Position = mul(Translation, float4(ColorR, ColorG, ColorB, 1.0f));
	OUT.Position = mul(worldViewProjectionMatrix, OUT.Position);

	OUT.Color = float3(ColorR, ColorG, ColorB);

	return OUT;

}