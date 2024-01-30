struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 texcoord : TEXCOORD;
	float32_t3 normal : NORMAL0;
	float32_t3 worldPosition : POSITION0;
};
struct DirectionalLight
{
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};

struct PointLight
{
	float32_t4 color;
	float32_t3 position;
	float intensity;
	float radius;
	float decay;
	int32_t isUse;
};

struct SpotLight
{
	float32_t4 color;
	float32_t3 position;
	float intensity;
	float32_t3 direction;
	float distance;
	float decay;
	float cosAngle;
};