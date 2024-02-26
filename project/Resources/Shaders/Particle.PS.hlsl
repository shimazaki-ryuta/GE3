#include "Particle.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material {
	float32_t4 color;
	float32_t4x4 uvTransform;
	float32_t growStrength;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 grow : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input) {
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	PixelShaderOutput output;
	//output.color = gMaterial.color * textureColor;
	output.color = gMaterial.color * textureColor * input.color;
	output.grow = gMaterial.color * textureColor * input.color * gMaterial.growStrength;
	if (output.color.a == 0.0)
	{
		discard;
	}
	return output;
}