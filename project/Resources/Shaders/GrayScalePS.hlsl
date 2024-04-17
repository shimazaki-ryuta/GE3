#include "Blume.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material {
	float32_t4 color;
	float32_t4x4 uvTransform;

};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(SpriteVertexShaderOutput input) {
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t3 textureColor = gTexture.Sample(gSampler, transformedUV.xy).rgb;
	float value = dot(textureColor,float32_t3(0.2125f,0.7154f,0.0721f));
	float32_t3 grayScaleColor = float32_t3(value, value, value);
	PixelShaderOutput output;
	//output.color = gMaterial.color * textureColor;

	float t = gMaterial.color.a;

	output.color.rgb = (1.0f - t) * textureColor + t* grayScaleColor;

	output.color.a = gTexture.Sample(gSampler, transformedUV.xy).a;
	
	return output;
}