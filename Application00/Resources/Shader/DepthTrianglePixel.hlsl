//--------------------------------------------------------------------------------------
// PixelShader.hlsl
//
// Simple shader to render a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct Interpolants
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

Pixel main( Interpolants In )
{
    Pixel Out;
    Out.color = In.color;
    //Out.color = float4(1.0f,1.0f,1.0f,1.0f);
    return Out;
}