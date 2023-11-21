#include "Object3d.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};
struct Camera {
	float32_t3 worldPosition;
};
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
	float32_t3 reflectLight = reflect(gDirectionalLight.direction,normalize(input.normal));
	float RdotE = dot(reflectLight,toEye);
	float specularPow = pow(saturate(RdotE),gMaterial.shininess);
	float32_t3 diffuse;
	float32_t3 specular;
	PixelShaderOutput output;
	output.color.a = gMaterial.color.a * textureColor.a;
	if (textureColor.a <= 0.0)
	{
		discard;
	}
	if (gMaterial.enableLighting == 2)
	{
		float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
		diffuse = gMaterial.color.xyz * textureColor.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
		
		specular = gMaterial.color.rgb * gDirectionalLight.intensity * specularPow * gDirectionalLight.color.xyz;
		output.color.rgb = diffuse + specular;
	}else if(gMaterial.enableLighting == 1){
		float cos = saturate(dot(normalize(input.normal),-gDirectionalLight.direction));
		output.color.xyz = gMaterial.color.xyz * textureColor.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
	}
	else{
		output.color = gMaterial.color * textureColor;
	}
	//output.color = gMaterial.color * textureColor;
	if (output.color.a == 0.0)
	{
		discard;
	}
	return output;
}