#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define MAX_MULTILIGHT 5

#define DEGREETORADIAN 0.0174532925

struct Light
{
	float4 color;
	float3 position;
	float spot;
	float3 direction;
	float range;
	float intensity;
	bool enable;
	int lightType;
	bool castShadow;
};