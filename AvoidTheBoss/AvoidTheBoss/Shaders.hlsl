//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}

// �÷��̾� ���� ��� ���� ����
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
};

//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
};

//---------------------------------------------------------�÷��̾� ���

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//���� ���̴��� �����Ѵ�. 
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	//������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�.
	output.position = mul(mul(mul(
		float4(input.position, 1.0f), 
		gmtxPlayerWorld),
		gmtxView),
		gmtxProjection);

	//�ԷµǴ� �ȼ��� ����(�����Ͷ����� �ܰ迡�� �����Ͽ� ���� ����)�� �״�� ����Ѵ�. 
	output.color = input.color;

	return(output);
}

//�ȼ� ���̴��� �����Ѵ�. 
float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	//�ԷµǴ� �ȼ��� ������ �״�� ���-���� �ܰ�(���� Ÿ��)�� ����Ѵ�. 
	return(input.color);
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}

//---------------------------------------------------------�ؽ��� ���� ���� ��ü ���
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

//���� ���̴��� �����Ѵ�. 
VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	//������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�.
	output.position = mul(mul(mul(
		float4(input.position, 1.0f),
		gmtxGameObject),
		gmtxView),
		gmtxProjection);

	//�ԷµǴ� �ȼ��� ������ ����Ѵ�. 
	output.uv = input.uv;

	return(output);
}

//�ȼ� ���̴��� �����Ѵ�. 
float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
	//�ԷµǴ� �ȼ��� ������ ����Ѵ�. 
	//return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}