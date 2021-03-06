struct Vertex
{
    float3 position     : POSITION;
    float4 color        : COLOR;
};

struct Interpolants
{
    float4 position     : SV_POSITION;
    float4 color        : COLOR;
};

Interpolants main( Vertex input )
{
   Interpolants result;
   result.position = float4(input.position, 1.0f);
   result.color = input.color;
   return result;
}