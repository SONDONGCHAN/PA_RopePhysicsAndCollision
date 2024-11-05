#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector      g_vColor = {0.f, 0.f, 0.f, 0.f};
float       g_fThickness;
float       g_fSize;


struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition  : POSITION;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}


struct GS_IN
{
    float4 vPosition        : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
};


[maxvertexcount(12)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[8];

	matrix matVP    = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + float4(-g_fSize * 0.5f,  g_fThickness * 0.5f, 0.f, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);

    Out[1].vPosition = In[0].vPosition + float4( g_fSize * 0.5f,  g_fThickness * 0.5f, 0.f, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);

    Out[2].vPosition = In[0].vPosition + float4( g_fSize * 0.5f, -g_fThickness * 0.5f, 0.f, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);

    Out[3].vPosition = In[0].vPosition + float4(-g_fSize * 0.5f, -g_fThickness * 0.5f, 0.f, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);


    Out[4].vPosition = In[0].vPosition + float4(-g_fThickness * 0.5f, g_fSize * 0.5f, 0.f, 0.f);
    Out[4].vPosition = mul(Out[4].vPosition, matVP);

    Out[5].vPosition = In[0].vPosition + float4(g_fThickness * 0.5f, g_fSize * 0.5f, 0.f, 0.f);
    Out[5].vPosition = mul(Out[5].vPosition, matVP);

    Out[6].vPosition = In[0].vPosition + float4(g_fThickness * 0.5f, -g_fSize * 0.5f, 0.f, 0.f);
    Out[6].vPosition = mul(Out[6].vPosition, matVP);

    Out[7].vPosition = In[0].vPosition + float4(-g_fThickness * 0.5f, -g_fSize * 0.5f, 0.f, 0.f);
    Out[7].vPosition = mul(Out[7].vPosition, matVP);

    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();

    DataStream.Append(Out[4]);
    DataStream.Append(Out[5]);
    DataStream.Append(Out[6]);
    DataStream.RestartStrip();

    DataStream.Append(Out[4]);
    DataStream.Append(Out[6]);
    DataStream.Append(Out[7]);
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    //float2 vTexcoord : TEXCOORD0;
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
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}


