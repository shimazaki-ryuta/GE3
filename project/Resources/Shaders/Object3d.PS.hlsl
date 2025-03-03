#include "Object3d.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gToonShadowTexture : register(t1);
TextureCube<float32_t4> gEnvironmentTexture : register(t3);
Texture2D<float32_t> disolveMaskTexture : register(t4);
SamplerState gSampler : register(s0);
SamplerState gSampler2 : register(s1);
struct Material {
	float32_t4 color;
	float32_t4 disolveColor;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
	float32_t growStrength;
	float32_t environmentCoefficient;
	float32_t disolveThreshold;
	int32_t shadingType;
};
struct Camera {
	float32_t3 worldPosition;
};
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
StructuredBuffer<PointLight> gPointLight : register(t2);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 grow : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input){
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t4 textureColorNotSampler = gTexture.Sample(gSampler2, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
	float32_t3 reflectLight = reflect(gDirectionalLight.direction,normalize(input.normal));
	float RdotE = dot(reflectLight,toEye);
	float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
	float NDotH = dot(normalize(input.normal),halfVector);
	float speculaStrength = max(0.001f,gMaterial.shininess);
	float specularPow = pow(saturate(NDotH),speculaStrength * 100.0f) * speculaStrength;
	float32_t3 diffuseDirectionalLight;
	float32_t3 specularDirectionalLight;
	float32_t3 diffusePointLight;
	float32_t3 specularPointLight;
	float32_t3 diffuseSpotLight;
	float32_t3 specularSpotLight;
	float32_t3 diffuseBrightness=0;
	float32_t3 speculaBrightness = 0;
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
		diffuseBrightness += gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
		specularDirectionalLight = gMaterial.color.rgb * gDirectionalLight.intensity * specularPow * gDirectionalLight.color.xyz;
		speculaBrightness += gDirectionalLight.intensity * specularPow * gDirectionalLight.color.xyz;
		
	
		diffusePointLight = 0;
		specularPointLight = 0;
		for (int i = 0; i < 32;i++) {
			if (gPointLight[i].isUse) {
				float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight[i].position);
				float NdotLPoint = dot(normalize(input.normal), -pointLightDirection);
				float cosPoint = pow(NdotLPoint * 0.5f + 0.5f, 2.0f);
				float32_t distance = length(gPointLight[i].position - input.worldPosition);
				float32_t factor = pow(saturate(-distance / gPointLight[i].radius + 1.0f), gPointLight[i].decay);
				halfVector = normalize(-pointLightDirection + toEye);
				NDotH = dot(normalize(input.normal), halfVector);
				specularPow = pow(saturate(NDotH), speculaStrength * 100.0f) * speculaStrength;

				diffusePointLight += gMaterial.color.rgb * textureColor.xyz * gPointLight[i].color.rgb * cosPoint * gPointLight[i].intensity * factor;
				diffuseBrightness += gPointLight[i].color.rgb * cosPoint * gPointLight[i].intensity * factor;
				specularPointLight += gMaterial.color.rgb * gPointLight[i].intensity * specularPow * gPointLight[i].color.xyz * factor;
				speculaBrightness += gPointLight[i].intensity * specularPow * gPointLight[i].color.xyz * factor;
			}
		}
		

		float32_t3 spotLightDirection = normalize(input.worldPosition - gSpotLight.position);
		float NdotLspot = dot(normalize(input.normal), -spotLightDirection);
		float cosspot = pow(NdotLspot * 0.5f + 0.5f, 2.0f);
		float32_t distance2 = length(gSpotLight.position - input.worldPosition);
		float32_t factor2 = pow(saturate(-distance2 / gSpotLight.distance + 1.0f), gSpotLight.decay);
		halfVector = normalize(-spotLightDirection + toEye);
		NDotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NDotH), speculaStrength)  * speculaStrength;

		float32_t cosAngle = dot(spotLightDirection,gSpotLight.direction);
		float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle)/(1.0f - gSpotLight.cosAngle));

		diffuseSpotLight = gMaterial.color.rgb * textureColor.xyz * gSpotLight.color.rgb * gSpotLight.intensity * factor2 * falloffFactor;
		diffuseBrightness += gSpotLight.color.rgb * gSpotLight.intensity * factor2 * falloffFactor;
		specularSpotLight = gMaterial.color.rgb * gSpotLight.intensity * specularPow * gSpotLight.color.xyz * factor2 * falloffFactor;
		speculaBrightness += gSpotLight.intensity * specularPow * gSpotLight.color.xyz * factor2 * falloffFactor;

		if (gMaterial.shadingType == 0) {
			output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
		else if (gMaterial.shadingType == 1) {
			output.color = gMaterial.color * textureColor;
			float32_t3 lightColor = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
			float32_t3 sumBrightness = diffuseBrightness;
			float brightness = sqrt(pow(sumBrightness.r, 2) + pow(sumBrightness.g, 2) + pow(sumBrightness.b, 2));
			brightness = saturate(brightness);
			float32_t3 color = gMaterial.color.xyz * textureColor.xyz;
			float sub = color.r - color.g;
			float sub2 = color.r - color.b;
			float sub3 = color.g - color.b;
			float32_t2 shadowTexcoord = {brightness,brightness };
			float32_t4 shadowColor = gToonShadowTexture.Sample(gSampler2,shadowTexcoord);
			output.color.rgb = output.color.rgb * normalize(lightColor) * shadowColor.rgb;
		}
	
	}else if(gMaterial.enableLighting == 1){
		float cos = saturate(dot(normalize(input.normal),-gDirectionalLight.direction));
		output.color.xyz = gMaterial.color.xyz * textureColor.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
	}
	else{
		output.color = gMaterial.color * textureColor;
	}
	if (output.color.a == 0.0)
	{
		discard;
	}
	if(gMaterial.environmentCoefficient != 0.0f){
		float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
		float32_t3 refrectedVector = reflect(cameraToPosition,normalize(input.normal));
		float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler,refrectedVector);
		environmentColor.rgb *= gMaterial.environmentCoefficient;
		output.color.rgb += environmentColor.rgb;
	}
	//disolve
	{
		float32_t mask = disolveMaskTexture.Sample(gSampler,transformedUV.xy);
		float min = gMaterial.disolveThreshold-0.03f;
		float edge = 1.0f - smoothstep(min,min+0.06f,mask);
		edge *=2.0f;
		edge -=1.0f;
		if(mask <= min){
			discard;
		}
		edge = (1.0f-pow(edge,2));
		edge *= pow(edge,4);
		edge+=0.1f;
		if(min < mask && mask < min+0.06f && gMaterial.disolveThreshold!=0.0f){
			output.color.rgb = edge * gMaterial.disolveColor.rgb * 3.0f;
		}
	}
	output.grow = gMaterial.color * textureColor * gMaterial.growStrength;
	return output;
}