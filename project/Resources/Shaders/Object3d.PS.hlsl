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
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
	float32_t3 reflectLight = reflect(gDirectionalLight.direction,normalize(input.normal));
	float RdotE = dot(reflectLight,toEye);
	float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
	float NDotH = dot(normalize(input.normal),halfVector);
	float specularPow = pow(saturate(NDotH),gMaterial.shininess);
	float32_t3 diffuseDirectionalLight;
	float32_t3 specularDirectionalLight;
	float32_t3 diffusePointLight;
	float32_t3 specularPointLight;
	float32_t3 diffuseSpotLight;
	float32_t3 specularSpotLight;
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
		diffuseDirectionalLight = gMaterial.color.xyz * textureColor.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
		
		specularDirectionalLight = gMaterial.color.rgb * gDirectionalLight.intensity * specularPow * gDirectionalLight.color.xyz;
		
		float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
		float NdotLPoint = dot(normalize(input.normal), -pointLightDirection);
		float cosPoint = pow(NdotLPoint * 0.5f + 0.5f, 2.0f);
		float32_t distance = length(gPointLight.position - input.worldPosition);
		float32_t factor = pow(saturate(-distance / gPointLight.radius+1.0f),gPointLight.decay);
		halfVector = normalize(-pointLightDirection + toEye);
		NDotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NDotH), gMaterial.shininess);
		diffusePointLight = gMaterial.color.rgb * textureColor.xyz * gPointLight.color.rgb * cosPoint * gPointLight.intensity * factor;
		specularPointLight = gMaterial.color.rgb * gPointLight.intensity * specularPow * gPointLight.color.xyz * factor;


		float32_t3 spotLightDirection = normalize(input.worldPosition - gSpotLight.position);
		float NdotLspot = dot(normalize(input.normal), -spotLightDirection);
		float cosspot = pow(NdotLspot * 0.5f + 0.5f, 2.0f);
		float32_t distance2 = length(gSpotLight.position - input.worldPosition);
		float32_t factor2 = pow(saturate(-distance2 / gSpotLight.distance + 1.0f), gSpotLight.decay);
		halfVector = normalize(-spotLightDirection + toEye);
		NDotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NDotH), gMaterial.shininess);

		float32_t cosAngle = dot(spotLightDirection,gSpotLight.direction);
		float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle)/(1.0f - gSpotLight.cosAngle));

		diffuseSpotLight = gMaterial.color.rgb * textureColor.xyz * gSpotLight.color.rgb * gSpotLight.intensity * factor2 * falloffFactor;
		specularSpotLight = gMaterial.color.rgb * gSpotLight.intensity * specularPow * gSpotLight.color.xyz * factor2 * falloffFactor;


		output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
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