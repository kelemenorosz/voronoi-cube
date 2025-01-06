
cbuffer WorldProjectionMatrix : register(b0) {

	matrix worldProjectionMatrix;

};

struct VertexShaderInput {

	float4 Position : POSITION;
	float4 Color : COLOR;

};

struct VertexShaderOutput {

	float4 Color : COLOR;
	float4 Position : SV_Position;

};

static matrix <float, 4, 4> IdentityMatrix = {

	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f

};


static vector <float, 3> CameraPosition = {0.0f, 0.0f, -1.0f};
static matrix <float, 4, 4> WorldMatrix = {

	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,

};

static float farZ = 100.0f;
static float nearZ = 0.1f;
static float fov = 90.0f;
static float aspectWIDTH = 800.0f;
static float aspectHEIGHT = 600.0f;
static matrix <float, 4, 4> ProjectionMatrix = {

	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,

};

static float RotationAngle = radians(0.0f);

VertexShaderOutput main(VertexShaderInput IN)
{
	//Declare output struct
	VertexShaderOutput OUT;

	//Rotation around the X-axis
	matrix<float, 4, 4> RotationX = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, cos(RotationAngle), -sin(RotationAngle), 0.0f,
	0.0f, sin(RotationAngle), cos(RotationAngle), 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	//Rotation around the Y-axis
	matrix<float, 4, 4> RotationY = {
	cos(RotationAngle), 0.0f, -sin(RotationAngle), 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	sin(RotationAngle), 0.0f, cos(RotationAngle), 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	//Declare Model World Projection Matrix
	matrix <float, 4, 4> ModelWorldProjectionMatrix = IdentityMatrix;

	//World Matrix
	WorldMatrix._41 = -CameraPosition.x;
	WorldMatrix._42 = -CameraPosition.y;
	WorldMatrix._43 = -CameraPosition.z;

	//Projection Matrix calculations
	float aspect = aspectWIDTH / aspectHEIGHT;
	float depth = farZ - nearZ;
	float fovRadian = radians(fov);

	//Projection Matrix
	ProjectionMatrix._22 = 1.0f / tan(fovRadian / 0.5f);
	ProjectionMatrix._11 = -1.0f * (ProjectionMatrix._22 / aspect);
	ProjectionMatrix._33 = farZ / depth;
	ProjectionMatrix._34 = (-farZ * nearZ) / depth;
	ProjectionMatrix._43 = 1;
	ProjectionMatrix._44 = 0;

	//Make Model World Projection Matrix
	ModelWorldProjectionMatrix = mul(WorldMatrix, ProjectionMatrix);

	OUT.Color = IN.Color;
	//OUT.Position = mul(ModelWorldProjectionMatrix, IN.Position);
	matrix<float, 4, 4> Rotation = mul(RotationX, IdentityMatrix);
	Rotation = mul(worldProjectionMatrix, Rotation);
	OUT.Position = mul(Rotation, IN.Position);
	
	return OUT;
}