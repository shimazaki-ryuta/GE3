#include "SkyBox.hlsli"


struct Material {
	float32_t4 color;
	float32_t3 expornenntiation;
};
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
TextureCube<float32_t4> gTexture : register(t0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler,input.texcoord);
	textureColor = textureColor *gMaterial.color;
	textureColor.r = textureColor.r * pow(textureColor.r*2.0f - 1.0f,gMaterial.expornenntiation.r);
	textureColor.g =textureColor.g* pow(textureColor.g*2.0f - 1.0f,gMaterial.expornenntiation.g);
	textureColor.b =textureColor.b* pow(textureColor.b*2.0f - 1.0f,gMaterial.expornenntiation.b);
	output.color = textureColor;
	//output.color.rgb = (tan((saturate(output.color.rgb) -0.5f)*3.141592f * 0.33f) );

	return output;
}