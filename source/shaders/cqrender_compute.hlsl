
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

float3 ConvertToLinear(float3 x)
{
	return x < 0.04045f ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

float3 ConvertToSRGB(float3 x)
{
	return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;
}

[numthreads(1, 1, 1)]
void main(ComputeShaderInput IN)
{

	/*float rColorFloat = ((float)InputImage[IN.DispatchThreadID.xy].x) / 255.0f;
	float gColorFloat = ((float)InputImage[IN.DispatchThreadID.xy].y) / 255.0f;
	float bColorFloat = ((float)InputImage[IN.DispatchThreadID.xy].z) / 255.0f;

	float3 ConvertedColors = ConvertToLinear(float3(rColorFloat, gColorFloat, bColorFloat));

	uint rColorUint = (uint)(ConvertedColors.x * 255.0f);
	uint gColorUint = (uint)(ConvertedColors.y * 255.0f);
	uint bColorUint = (uint)(ConvertedColors.z * 255.0f);*/

	float xTexel = ((float)IN.DispatchThreadID.x) * 2.0f + 1.0f;
	float yTexel = ((float)IN.DispatchThreadID.y) * 2.0f + 1.0f;

	xTexel = xTexel / 1280.0f;
	yTexel = yTexel / 720.0f;

	float2 TexelCoordinates = {xTexel, yTexel};

	float4 SampleColors = InputImage.SampleLevel(BilinearSampler, TexelCoordinates, 0);

	/*uint rColor = (uint)(SampleColors.z * 255.0f);
	uint gColor = (uint)(SampleColors.y * 255.0f);
	uint bColor = (uint)(SampleColors.x * 255.0f);
	uint aColor = (uint)(SampleColors.w * 255.0f);*/

	/*uint rColor = InputImage[IN.DispatchThreadID.xy].z;
	uint gColor = InputImage[IN.DispatchThreadID.xy].y;
	uint bColor = InputImage[IN.DispatchThreadID.xy].x;
	uint aColor = InputImage[IN.DispatchThreadID.xy].w;*/

	//OutputImage[IN.DispatchThreadID.xy] = uint4(rColor, gColor, bColor, aColor);
	float4 OutputColors = {

		SampleColors.z,
		SampleColors.y,
		SampleColors.x,
		SampleColors.w,
	
	};

	OutputImage[IN.DispatchThreadID.xy] = OutputColors;

	return;

}