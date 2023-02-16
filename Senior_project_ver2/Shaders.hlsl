float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

cbuffer cbPlayerInfo : register(b0)
{
	matrix	gmtxPlayerWorld : packoffset(c0);
}

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
};

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//정점 셰이더를 정의한다. 
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;

	//정점을 변환(월드 변환, 카메라 변환, 투영 변환)한다.
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView),
		gmtxProjection);
	//입력되는 픽셀의 색상(래스터라이저 단계에서 보간하여 얻은 색상)을 그대로 출력한다. 
	output.color = input.color;
	
	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	//입력되는 픽셀의 색상을 그대로 출력-병합 단계(렌더 타겟)로 출력한다. 
	return(input.color);
}

//텍스쳐  셰이더 정의
Texture2D gtxtTexture : register(t0); //baseregister t0
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}