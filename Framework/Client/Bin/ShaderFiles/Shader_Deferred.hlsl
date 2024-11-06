#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;

matrix		g_LightViewMatrix, g_LightProjMatrix;

texture2D	g_Texture;

vector		g_vLightDir;
vector		g_vLightPos;
float		g_fLightRange;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;


texture2D	g_NormalTexture;
texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularTexture;
texture2D	g_ShadowTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	/* 명암을 그린다. 빛을 받는 부분은 흰색, 안받는 부분은 검정색 */
	vector vShade : SV_TARGET0;	
	vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
	vector		vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
	/* 0 ~ 1 -> -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * 
		saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + 
		(g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));

	/* vWorldPos : 현재 렌더링 상태는 디퍼드로 수행한다. 디퍼드 화면에 대한 셰이딩 */
	/* Pixel셰이더에 입력된 픽셀이 어떤 객체의 픽셀인지 알 방법 없다. */
	vector		vWorldPos;

	/* 투영스페이스 상의 위치다. */
	/* 0 -> -1 */
	/* 1 -> 1 */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;

	/* 0 -> 1 */
	/* 1 -> -1 */
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;

	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;


	float		fViewZ = vDepthDesc.y * 1000.f;
	/* 투영행렬까지만 곱한 위치. */
	vWorldPos = vWorldPos * fViewZ;

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
	vector		vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);	
	vector		vWorldPos;	

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	float		fViewZ = vDepthDesc.y * 1000.f;
	
	vWorldPos = vWorldPos * fViewZ;	
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);	
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);
		
	/* 빛의 감쇠값을 설정한다. */
	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

	Out.vShade = g_vLightDiffuse *
		saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) +
		(g_vLightAmbient * g_vMtrlAmbient)) * 1.2f * fAtt ;

	vector		vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f) * fAtt;

	return Out;
	
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
	if (0.0f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);
	vector		vSpecular = g_SpecularTexture.Sample(g_LinearSampler, In.vTexcoord);

	vector		vColor = vDiffuse * vShade /* + vSpecular*/;

	vector		vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
	vector		vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	float		fViewZ = vDepthDesc.y * 1000.f;

	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vPosition = mul(vWorldPos, g_LightViewMatrix);
	vPosition = mul(vPosition, g_LightProjMatrix);

	float		fLightViewZ = vPosition.w;

	vPosition = vPosition.xyzw / vPosition.w;

	float2	vUV;

	/* -1 -> 0*/
	/* 1 -> 1 */
	vUV.x = vPosition.x * 0.5f + 0.5f;

	/* 1 -> 0*/
	/* -1 -> 1 */
	vUV.y = vPosition.y * -0.5f + 0.5f;

	vector		vLightDepth = g_ShadowTexture.Sample(g_PointSampler, vUV);

	if (vLightDepth.x * 2000.f + 0.1f <= fLightViewZ)
	{
		vColor = vColor * 0.7f;
	}

	Out.vColor = vColor;






	return Out;
}

technique11	DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred_Final
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}
}


