#include "Blume.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gGrowMap : register(t1);
SamplerState gSampler : register(s0);

struct Material {
	float32_t4 color;
	float32_t4x4 uvTransform;

};
ConstantBuffer<Material> gMaterial : register(b0);

static const float32_t PI = 3.14159265f;

float gauss(float x,float y, float sigma){
	float exponent = -(x*x + y*y) * rcp(2.0f * sigma * sigma);
	float denominator = 2.0f * PI * sigma * sigma;
	return exp(exponent) * rcp(denominator);
}

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(SpriteVertexShaderOutput input) {
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	PixelShaderOutput output;
	//output.color = gMaterial.color * textureColor;
	//output.color.a = gMaterial.color.a;
	output.color.rgb = float32_t3(0.0f,0.0f,0.0f);
	output.color.a = 0.0f;
	float32_t weight = 0.0f;
	float32_t kernel[35][35];
	int halfKernel=17;
	uint32_t width,height;
	gTexture.GetDimensions(width,height);
	float32_t2 uvStepSize = float32_t2(rcp(width),rcp(height));
	for (int y = -halfKernel; y <= halfKernel;y++) {
		for (int x = -halfKernel; x <= halfKernel; x++) {
			kernel[y+halfKernel][x+halfKernel] = gauss(float32_t(y),float32_t(x),60.0f);
			weight += kernel[y+halfKernel][x+halfKernel];
			float32_t2 texcoord = input.texcoord + float32_t2(float32_t(y),float32_t(x)) * uvStepSize;
			float32_t3 fetchColor = gGrowMap.Sample(gSampler,texcoord).rgb;
			//float32_t3 fetchColor = gTexture.Sample(gSampler,texcoord).rgb;
			output.color.rgb += fetchColor*kernel[y+halfKernel][x+halfKernel];
			/*
			float4 targetUV = transformedUV;
			targetUV.x += float(x) * (1.0f / 1280.0f);
			targetUV.y += float(y) * (1.0f / 720.0f);
			output.color += gGrowMap.Sample(gSampler, targetUV.xy) / (11.0f*11.0f);
			*/
		}
	}
	output.color.rgb *= rcp(weight);
	output.color.rgb += textureColor.rgb;
	output.color.a = gMaterial.color.a;
	return output;
}