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

[numthreads(1, 1, 1)]
void main(ComputeShaderInput IN) {

	if (CentroidBuffer[IN.DispatchThreadID.x].Count == 0) {

		return;

	}	

	float FloatSumR = (float)(CentroidBuffer[IN.DispatchThreadID.x].UintSum.x) / 255.0f;
	float FloatSumG = (float)(CentroidBuffer[IN.DispatchThreadID.x].UintSum.y) / 255.0f;
	float FloatSumB = (float)(CentroidBuffer[IN.DispatchThreadID.x].UintSum.z) / 255.0f;

	CentroidBuffer[IN.DispatchThreadID.x].Color.x = FloatSumR / (float)CentroidBuffer[IN.DispatchThreadID.x].Count;
	CentroidBuffer[IN.DispatchThreadID.x].Color.y = FloatSumG / (float)CentroidBuffer[IN.DispatchThreadID.x].Count;
	CentroidBuffer[IN.DispatchThreadID.x].Color.z = FloatSumB / (float)CentroidBuffer[IN.DispatchThreadID.x].Count;

	CentroidBuffer[IN.DispatchThreadID.x].Count = 0;
	CentroidBuffer[IN.DispatchThreadID.x].Sum.x = 0.0f;
	CentroidBuffer[IN.DispatchThreadID.x].Sum.y = 0.0f;
	CentroidBuffer[IN.DispatchThreadID.x].Sum.z = 0.0f;
	CentroidBuffer[IN.DispatchThreadID.x].UintSum.x = 0.0f;
	CentroidBuffer[IN.DispatchThreadID.x].UintSum.y = 0.0f;
	CentroidBuffer[IN.DispatchThreadID.x].UintSum.z = 0.0f;

	return;

}