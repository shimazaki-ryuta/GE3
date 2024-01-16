#include "outline.hlsli"

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	output.color = input.color;
	if (output.color.a == 0.0)
	{
		discard;
	}
	return output;
}