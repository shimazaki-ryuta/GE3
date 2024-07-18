#include "SkyBox.hlsli"


struct Material {
	float32_t4 color;
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
	//textureColor = textureColor* textureColor;
	textureColor.r = pow(textureColor.r,2.2f);
	textureColor.g = pow(textureColor.g,2.2f);
	textureColor.b = pow(textureColor.b,2.2f);
	output.color = textureColor  *gMaterial.color;
	
	return output;
}