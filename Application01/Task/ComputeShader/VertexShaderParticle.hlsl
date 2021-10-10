//--------------------------------------------------------------------------------------
// VertexShader.hlsl
//
// Simple vertex shader for rendering a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct Vertex
{
    float2 position     : Position;
};

struct VertexOutput
{
    float4 position     : SV_Position;
    float life        : COLOR0;
};

Texture2D<float4> txData : register( t0 );
SamplerState samLinear : register(s0);

VertexOutput main( Vertex input )
{
   float4 source = txData.SampleLevel( samLinear, input.position, 0 );

   VertexOutput output;
   output.position = float4(source.x, source.y, source.z, 1.0f);
   output.life = source.w;
   return output;
}