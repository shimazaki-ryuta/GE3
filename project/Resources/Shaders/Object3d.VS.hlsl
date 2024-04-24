#include "Object3d.hlsli"

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct LocalMatrix {
	float32_t4x4 m;
};
ConstantBuffer<LocalMatrix> gLocalMatrix : register(b8);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 center : CENTER0;
};

VertexShaderOutput main(VertexShaderInput input){
	VertexShaderOutput output;
	output.position = mul(input.position, mul(gLocalMatrix.m,gTransformationMatrix.WVP));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal,(float32_t3x3)mul(gLocalMatrix.m,gTransformationMatrix.WorldInverseTranspose)));
	output.worldPosition = mul(input.position, mul(gLocalMatrix.m,gTransformationMatrix.World)).xyz;
	return output;
}