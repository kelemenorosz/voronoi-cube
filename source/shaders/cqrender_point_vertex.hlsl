
cbuffer WorldProjectionMatrix : register(b0) {

	matrix worldProjectionMatrix;

};

struct VertexShaderInput {

	uint PointData : R_POSITION;

};

struct VertexShaderOutput {

	float4 Color : COLOR;
	float4 Position : SV_Position;

};

float rand1(float n) { return frac(sin(n) * 43758.5453123); }



VertexShaderOutput main(VertexShaderInput IN) {

	//Declare output struct
	VertexShaderOutput OUT;

	//Get red, green and blue 8-bit values
	uint Red;
	uint Green;
	uint Blue;
	Red   = (IN.PointData & 0x00ff0000) >> 16;
	Green = (IN.PointData & 0x0000ff00) >> 8;
	Blue  = (IN.PointData & 0x000000ff);

	//Map to cube coordinates
	float xPos = ((float)Red - 128.0f) / 128.0f;
	float yPos = (float)(Green - 128.0f) / 128.0f;
	float zPos = (float)(Blue - 128.0f) / 128.0f;

	float rColor = ((float)Red) / 256.0f;
	float gColor = ((float)Green) / 256.0f;
	float bColor = ((float)Blue) / 256.0f;

	vector<float, 4> Pos = {xPos, yPos, zPos, 1.0f};
	float4 Color = {rColor, gColor, bColor, 1.0f};

	OUT.Position = mul(worldProjectionMatrix, Pos);
	OUT.Color = Color;

	return OUT;

}