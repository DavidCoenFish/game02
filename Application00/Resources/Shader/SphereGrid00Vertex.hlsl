//--------------------------------------------------------------------------------------
struct Vertex
{
    float2 position     : POSITION;
    float2 uv           : TEXCOORD;
};

struct Interpolants
{
    float4 position     : SV_POSITION;
    float2 uv           : TEXCOORD;
};

Interpolants main( Vertex input )
{
   Interpolants result;
   result.position = float4(input.position, 0.0f, 1.0f);
   result.uv = input.uv;
   return result;
}