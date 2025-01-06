
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

Texture2D<float4> PictureTexture : register(t0);
RWStructuredBuffer<Centroid> CentroidBuffer : register(u0);

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

		DistanceX = CentroidBuffer[i].Color.x - TexelColor.x;
		DistanceY = CentroidBuffer[i].Color.y - TexelColor.y;
		DistanceZ = CentroidBuffer[i].Color.z - TexelColor.z;

		Distance = sqrt(DistanceX * DistanceX + DistanceY * DistanceY + DistanceZ * DistanceZ);
		if (Distance < MinDistance) {

			MinDistance = Distance;
			MinDistanceIndex = i;

		}

	}

	InterlockedAdd(CentroidBuffer[MinDistanceIndex].Count, 1);
	//CentroidBuffer[MinDistanceIndex].Count += 1;
	
	uint UintTexelColorR = (uint)(TexelColor.x * 255.0f);
	uint UintTexelColorG = (uint)(TexelColor.y * 255.0f);
	uint UintTexelColorB = (uint)(TexelColor.z * 255.0f);

	InterlockedAdd(CentroidBuffer[MinDistanceIndex].UintSum.x, UintTexelColorR);
	InterlockedAdd(CentroidBuffer[MinDistanceIndex].UintSum.y, UintTexelColorG);
	InterlockedAdd(CentroidBuffer[MinDistanceIndex].UintSum.z, UintTexelColorB);

	/*CentroidBuffer[MinDistanceIndex].Sum.x = CentroidBuffer[MinDistanceIndex].Sum.x + TexelColor.x;
	CentroidBuffer[MinDistanceIndex].Sum.y = CentroidBuffer[MinDistanceIndex].Sum.y + TexelColor.y;
	CentroidBuffer[MinDistanceIndex].Sum.z = CentroidBuffer[MinDistanceIndex].Sum.z + TexelColor.z;*/

	return;

}