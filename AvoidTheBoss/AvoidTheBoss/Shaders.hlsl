//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}

// 플레이어 정보 상수 버퍼 선언
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
};

//---------------------------------------------------------플레이어 출력

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//정점 셰이더를 정의한다. 
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	//정점을 변환(월드 변환, 카메라 변환, 투영 변환)한다.
	output.position = mul(mul(mul(
		float4(input.position, 1.0f), 
		gmtxPlayerWorld),
		gmtxView),
		gmtxProjection);

	//입력되는 픽셀의 색상(래스터라이저 단계에서 보간하여 얻은 색상)을 그대로 출력한다. 
	output.color = input.color;

	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	//입력되는 픽셀의 색상을 그대로 출력-병합 단계(렌더 타겟)로 출력한다. 
	return(input.color);
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}

//---------------------------------------------------------텍스쳐 적용 게임 객체 출력
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

//정점 셰이더를 정의한다. 
VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	//정점을 변환(월드 변환, 카메라 변환, 투영 변환)한다.
	output.position = mul(mul(mul(
		float4(input.position, 1.0f),
		gmtxGameObject),
		gmtxView),
		gmtxProjection);

	//입력되는 픽셀의 색상을 출력한다. 
	output.uv = input.uv;

	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
	//입력되는 픽셀의 색상을 출력한다. 
	//return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}