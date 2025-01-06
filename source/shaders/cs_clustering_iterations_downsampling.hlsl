
struct ComputeShaderInput
{
	uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

Texture2D<float4> InputImage : register(t0);
RWTexture2D<float4> OutputImage : register(u0);
SamplerState BilinearSampler : register(s0);

[numthreads(8, 8, 1)]
void main(ComputeShaderInput IN)
{

	if (IN.DispatchThreadID.x < TEXTURE_WIDTH && IN.DispatchThreadID.y < TEXTURE_HEIGHT) {

		float xTexel = ((float)IN.DispatchThreadID.x) * 2.0f + 1.0f;
		float yTexel = ((float)IN.DispatchThreadID.y) * 2.0f + 1.0f;

		xTexel = xTexel / TEXTURE_WIDTH;
		yTexel = yTexel / TEXTURE_HEIGHT;

		float2 TexelCoordinates = { xTexel, yTexel };

		float4 SampleColors = InputImage.SampleLevel(BilinearSampler, TexelCoordinates, 0);

		float4 OutputColors = {

		SampleColors.z,
		SampleColors.y,
		SampleColors.x,
		SampleColors.w,

		};

		OutputImage[IN.DispatchThreadID.xy] = OutputColors;

	}

}