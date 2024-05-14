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
	
	float32_t2 correct =  input.texcoord * (1.0f - input.texcoord.yx);
	float vignette = correct.x * correct.y * 16.0f;
	vignette = saturate(pow(vignette,0.6f));
	float32_t3 vignettingColor;
	vignettingColor.r = output.color.r * vignette + (1.0f - vignette)*0.1f;
	vignettingColor.gb = output.color.gb * vignette;
	output.color.rgb = (1.0f - t) * output.color.rgb + t* vignettingColor;
	return output;
}