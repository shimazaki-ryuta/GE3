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
	output.color = textureColor*gMaterial.color;
	
	return output;
}