#include "Blume.hlsli"
#include "RGB_HSVConvert.hlsli"
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
	PixelShaderOutput output;
	float32_t3 hsv = RGBToHSV(textureColor.rgb);
	hsv += (gMaterial.color.rgb);
	hsv.r = fmod(hsv.r,1.0f);
	output.color.rgb = HSVToRGB(hsv);
	output.color.a = gMaterial.color.a;
	return output;
}