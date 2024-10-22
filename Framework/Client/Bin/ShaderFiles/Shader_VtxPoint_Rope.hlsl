#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix1, g_WorldMatrix2;
matrix      g_ViewMatrix, g_ProjMatrix;

vector      g_vColor;
float       g_fThickness;

struct VS_IN
{
    float3 vPosition1 : POSITION0;
    float3 vPosition2 : POSITION1;
};

struct VS_OUT
{
    float4 vPosition1   : POSITION0;
    float4 vPosition2   : POSITION1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition1 = mul(vector(In.vPosition1, 1.f), g_WorldMatrix1);
    Out.vPosition2 = mul(vector(In.vPosition2, 1.f), g_WorldMatrix2);
    
    return Out;
}


struct GS_IN
{
    float4 vPosition1        : POSITION0;
    float4 vPosition2        : POSITION1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook    = (In[0].vPosition1 - In[0].vPosition2).xyz;
    float3 vRight   = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) ;
    float3 vUp      = normalize(cross(vLook, vRight));
	matrix matVP    = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition1 - float4(vRight * g_fThickness, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);

    Out[1].vPosition = In[0].vPosition1 + float4(vRight * g_fThickness, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition2 + float4(vRight * g_fThickness, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition2 - float4(vRight * g_fThickness, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
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
    PS_OUT Out = (PS_OUT) 0;
     
    Out.vColor = g_vColor;
    
  
    return Out;
}

/* EffectFramework */

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}


