//--------------------------------------------------------------------------------------
// VertexShader.hlsl
//
// Simple vertex shader for rendering a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

cbuffer VertexConstants : register(b0)
{
    float m_time;
};

struct Vertex
{
    float2 position     : Position;
    float4 color        : COLOR0;
};

struct VertexOutput
{
    float4 position     : SV_Position;
    float4 color        : COLOR0;
};

VertexOutput main( Vertex input )
{
   VertexOutput output;
   float tx = sin(m_time * 6.283185307179586476925286766559f);
   float ty = cos(m_time * 6.283185307179586476925286766559f);
   output.position = float4(
      (input.position.x * tx) + (input.position.y * ty), //dot with tx,ty
      (input.position.x * (-ty)) + (input.position.y * (tx)), //dot with (90deg rot tx,ty => (-ty),tx)
      0.0f, 
      1.0f);
   output.color = input.color;
   return output;
}