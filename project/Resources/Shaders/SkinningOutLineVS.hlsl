//#include "outline.hlsli"

struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 texcoord : TEXCOORD;
	float32_t3 normal : NORMAL0;
	float32_t3 worldPosition : POSITION0;
	float32_t4 color : COLOR0;
};

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
	float32_t4x4 ScaleInverse;

};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct OutLineData {
	float32_t4 Color;
	float32_t4x4 Scale;
};
ConstantBuffer<OutLineData> gOutLineData : register(b1);

struct Well{
	float32_t4x4 skeletonSpaceMatrix;
	float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t4);

struct Skinned{
	float32_t4 position;
	float32_t3 normal;
};

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

Skinned Skinning(VertexShaderInput input){
	Skinned skinned;

	//pos
	skinned.position = mul(input.position,gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
	skinned.position += mul(input.position,gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
	skinned.position += mul(input.position,gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
	skinned.position += mul(input.position,gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
	skinned.position.w = 1.0f;

	//normal
	skinned.normal = mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 wvp = gTransformationMatrix.WVP;
	float32_t4 interporationPosition;
	float32_t4 ln = 1;

	Skinned skinned = Skinning(input);

	output.position = skinned.position;
	output.texcoord = input.texcoord;
	output.normal = (mul(skinned.normal,(float32_t3x3)gTransformationMatrix.WorldInverseTranspose));

	//output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.ScaleInverse));
	ln.xyz = output.normal;
	interporationPosition = mul(ln, gOutLineData.Scale);
	
	interporationPosition += output.position;
	interporationPosition.w = 1.0f;
	output.position = mul(interporationPosition, wvp);
	output.texcoord = input.texcoord;
	output.color = gOutLineData.Color;
	return output;
}