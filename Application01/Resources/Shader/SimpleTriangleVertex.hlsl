struct Vertex
{
    float2 position     : POSITION;
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
   result.position = float4(input.position, 0.5f, 1.0f);
   result.color = input.color;
   return result;
}