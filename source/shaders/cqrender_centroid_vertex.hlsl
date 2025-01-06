

struct VertexShaderInput {

	float4 PointData : R_POSITION;

};

struct VertexShaderOutput {

	float4 Color : COLOR;
	float4 Position : SV_Position;

};

struct Centroid {

	float3 Color;
	float3 Sum;
	uint3  UintSum;
	uint   Count;

};

cbuffer WorldProjectionMatrix : register(b0) {

	matrix worldProjectionMatrix;

};
RWStructuredBuffer<Centroid> CentroidBuffer : register(u0);

VertexShaderOutput main(VertexShaderInput IN) {
	
	VertexShaderOutput OUT;

	float floatPosX = IN.PointData.x; //Values between 0.0f and 99.0f/255.0f
	float floatPosY = IN.PointData.y;
	float floatPosZ = IN.PointData.z;

	uint uintPosX = (uint)(floatPosX * 255.0f); //Values between 0 and 99
	uint uintPosY = (uint)(floatPosY * 255.0f);
	uint uintPosZ = (uint)(floatPosZ * 255.0f);

	float floatNormPosX = ((float)uintPosX) / 99.0f; //Values between 0.0f and 99.0f/99.0f
	float floatNormPosY = ((float)uintPosY) / 99.0f;
	float floatNormPosZ = ((float)uintPosZ) / 99.0f;

	float floatCubePosX = (((float)uintPosX) - 49.5f) / 49.5f; //Values between -49.5f/49.5f and 49.5f/49.5f
	float floatCubePosY = (((float)uintPosY) - 49.5f) / 49.5f;
	float floatCubePosZ = (((float)uintPosZ) - 49.5f) / 49.5f;

	
	float Distance = 0.0f;
	float MinDistance = 100.0f;
	uint  MinDistanceIndex = 0;

	float DistanceX = 0.0f;
	float DistanceY = 0.0f;
	float DistanceZ = 0.0f;

	for (uint i = 0; i < CENTROID_COUNT; i++) {

		DistanceX = floatNormPosX - CentroidBuffer[i].Color.x;
		DistanceY = floatNormPosY - CentroidBuffer[i].Color.y;
		DistanceZ = floatNormPosZ - CentroidBuffer[i].Color.z;

		Distance = sqrt(DistanceX * DistanceX + DistanceY * DistanceY + DistanceZ * DistanceZ);
		if (Distance < MinDistance) {

			MinDistance = Distance;
			MinDistanceIndex = i;

		}

	}

	/*
	uint Red   = (uint)(floatPosX * 255.0f);
	uint Green = (uint)(floatPosY * 255.0f);
	uint Blue  = (uint)(floatPosZ * 255.0f);

	float xPos = ((float)Red - 128.0f)   / 128.0f;
	float yPos = ((float)Green - 128.0f) / 128.0f;
	float zPos = ((float)Blue - 128.0f)  / 128.0f;*/

	//Map to cube coordinates
	/*float xPos = ((float)PosX - 128.0f) / 128.0f;
	float yPos = ((float)PosY - 128.0f) / 128.0f;
	float zPos = ((float)PosZ - 128.0f) / 128.0f;*/

	/*uint PosX = (IN.PointData & 0x00ff0000) >> 16;
	uint PosY = (IN.PointData & 0x00ff0000) >> 8;
	uint PosZ = IN.PointData & 0x000000ff;

	float xPos = ((float)PosX) / 99.0f;
	float yPos = ((float)PosY) / 99.0f;
	float zPos = ((float)PosZ) / 99.0f;*/

	vector<float, 4> Pos   = { floatCubePosX, floatCubePosY, floatCubePosZ, 1.0f };
	vector<float, 4> Color = { CentroidBuffer[MinDistanceIndex].Color.x, CentroidBuffer[MinDistanceIndex].Color.y, CentroidBuffer[MinDistanceIndex].Color.z, 0.0f };

	OUT.Position = mul(worldProjectionMatrix, Pos);
	OUT.Color = Color;

	return OUT;

}