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

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f}},
	{{-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
	{{-1.0f,1.0f},{0.0f,1.0f},{1.0f,1.0f}},
};

static const float32_t kKernel3x3[3][3]={
	{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},
	{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},
	{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},
};
/*
static const float32_t kKernel3x3[3][3]={
	{0.2f,0.05f,0.05f},
	{0.005f,0.0f,0.05f},
	{0.005f,0.05f,0.2f},
};
*/
PixelShaderOutput main(SpriteVertexShaderOutput input) {
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t3 textureColor = gTexture.Sample(gSampler, transformedUV.xy).rgb;
	/*
	float32_t2 center = float32_t2(0.5f,0.5f);
	int32_t numSamples = 16;
	float32_t blurWidth = 0.001f;
	float32_t2 direction = input.texcoord - center;
	textureColor.rgb = float32_t3(0.0f,0.0f,0.0f);
	for(int32_t index=0;index < numSamples;index++){
		float32_t2 texcoord = input.texcoord + direction * blurWidth * float32_t(index);
		textureColor.rgb += gTexture.Sample(gSampler,texcoord).rgb;
	}
	textureColor.rgb *= rcp(numSamples);
	*/
	
	//boxfilter
	/*uint32_t width,height;
	gTexture.GetDimensions(width,height);
	float32_t2 uvStepSize = float32_t2(rcp(width),rcp(height));
	textureColor.rgb = float32_t3(0.0f,0.0f,0.0f);
	for(int32_t x = 0;x < 3;++x){
		for(int32_t y = 0;y < 3;++y){
			float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
			float32_t3 fetchColor = gTexture.Sample(gSampler,texcoord).rgb;
			textureColor.rgb += fetchColor*kKernel3x3[x][y];
		}
	}*/
	

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

	//float32_t3 hsv = RGBToHSV(output.color.rgb);
	//hsv.r += 0.5f;
	//hsv.r = fmod(hsv.r,1.0f);
	//output.color.rgb = HSVToRGB(hsv);

	return output;
}