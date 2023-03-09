//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}
/*
struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

float4 Lighting(float3 vPosition, float3 vNormal)
{
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll(MAX_LIGHTS)] for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				cColor += DirectionalLight(i, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == POINT_LIGHT)
			{
				cColor += PointLight(i, vPosition, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == SPOT_LIGHT)
			{
				cColor += SpotLight(i, vPosition, vNormal, vToCamera);
			}
		}
	}
	cColor += (gcGlobalAmbientLight * gMaterial.m_cAmbient);
	cColor.a = gMaterial.m_cDiffuse.a;

	return(cColor);
}*/

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

cbuffer cbMapObjectInfo : register(b2)
{
	matrix gmtxMapObject : packoffset(c0);
	//MATERIAL gMaterial : packoffset(c4);
};

//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbGameObjectInfo : register(b3)
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
	//return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}
//---------------------------------------------------------�� �ε� ���

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

	//return(cColor);
	//�ԷµǴ� �ȼ��� ������ ����Ѵ�. 
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}


struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxMapObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxMapObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
//#ifdef _WITH_VERTEX_LIGHTING
//	return(input.color);
//#else
//	input.normalW = normalize(input.normalW);
//	float4 color = Lighting(input.positionW, input.normalW);
//
//	return(color);
//	
//
//#endif
	return(float4(1.0f, 0.0f, 1.0f, 1.0f));
}
