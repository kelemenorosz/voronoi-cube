
struct ComputeShaderInput
{
	uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

struct Centroid {

	float3 Color;
	float3 Sum;
	uint3  UintSum;
	uint   Count;

};

RWStructuredBuffer<Centroid> CentroidBuffer : register(u0);
RWTexture2D<float4> PictureTexture : register(u1);

[numthreads(8, 8, 1)]
void main(ComputeShaderInput IN) {

	if (IN.DispatchThreadID.x >= TEXTURE_WIDTH) {
		return;
	}

	if (IN.DispatchThreadID.y >= TEXTURE_HEIGHT) {
		return;
	}

	float Distance = 0.0f;
	float MinDistance = 100.0f;
	uint  MinDistanceIndex = 0;

	float4 TexelColorUnflipped = PictureTexture[IN.DispatchThreadID.xy];
	float3 TexelColor;

	TexelColor.x = TexelColorUnflipped.z;
	TexelColor.y = TexelColorUnflipped.y;
	TexelColor.z = TexelColorUnflipped.x;
	
	float DistanceX = 0.0f;
	float DistanceY = 0.0f;
	float DistanceZ = 0.0f;

	for (uint i = 0; i < CENTROID_COUNT; ++i) {

		DistanceX = TexelColor.x - CentroidBuffer[i].Color.x;
		DistanceY = TexelColor.y - CentroidBuffer[i].Color.y;
		DistanceZ = TexelColor.z - CentroidBuffer[i].Color.z;

		Distance = sqrt(DistanceX * DistanceX + DistanceY * DistanceY + DistanceZ * DistanceZ);
		if (Distance < MinDistance) {

			MinDistance = Distance;
			MinDistanceIndex = i;

		}

	}

	float4 OutputColor = {

		CentroidBuffer[MinDistanceIndex].Color.z,
		CentroidBuffer[MinDistanceIndex].Color.y,
		CentroidBuffer[MinDistanceIndex].Color.x,
		1.0f,
	};

	PictureTexture[IN.DispatchThreadID.xy] = OutputColor;

	return;

}