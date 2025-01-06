
cbuffer CameraPosition : register(b0) {

	float4 cameraPosition;

};

struct PixelShaderInput {

	float3 Color	: COLOR;
	float3 FragmentPosition : FPOSITION;
	float3 Normal	: NORMAL;

};

static float3 LightColor = {1.0f, 1.0f, 1.0f};
static float3 LightPosition = {-1.0f, 1.0f, -2.0f};

static float AmbientCoefficient = 0.5f;
static float SpecularStrength = 0.5f;
static float SpecularExponent = 64.0f;

float4 main(PixelShaderInput IN) : SV_Target
{

	float3 LightDirection = normalize(LightPosition - IN.FragmentPosition);
	float3 NormalizedNormal = normalize(IN.Normal);
	float DiffuseCoefficient = max(dot(LightDirection, NormalizedNormal), 0.0f);

	float3 ViewDirection = normalize(cameraPosition.xyz - IN.FragmentPosition);
	float3 ReflectionDirection = reflect(-LightDirection, NormalizedNormal);

	float SpecularCoefficient = pow(max(dot(ReflectionDirection, ViewDirection), 0.0f), SpecularExponent);

	float3 SpecularColor = SpecularCoefficient * SpecularStrength * LightColor;
	float3 AmbientColor = AmbientCoefficient * LightColor;
	float3 DiffuseColor = DiffuseCoefficient * LightColor;
	float3 FinalColor = (AmbientColor + DiffuseColor + SpecularColor) * IN.Color;

	return float4(FinalColor, 1.0f);
}