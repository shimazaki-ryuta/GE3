#include "Blume.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gGrowMap : register(t1);
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
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	PixelShaderOutput output;
	output.color = gMaterial.color * textureColor;
	output.color.a = gMaterial.color.a;
	for (int y = -5; y <= 5;y++) {
		for (int x = -5; x <= 5; x++) {
			float4 targetUV = transformedUV;
			targetUV.x += float(x) * (1.0f / 1280.0f);
			targetUV.y += float(y) * (1.0f / 720.0f);
			output.color += gGrowMap.Sample(gSampler, targetUV.xy) / (11.0f*11.0f);
		}
	}
	output.color.a = gMaterial.color.a;
	return output;
}