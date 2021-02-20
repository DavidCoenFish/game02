struct Interpolants
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

cbuffer PixelConstants : register(b0)
{
    float4 m_test;
};

Pixel main( Interpolants In )
{
    Pixel Out;
    Out.color = m_test;
    return Out;
}