#include "ReShade.fxh"

uniform float Sharpness <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 1.0;
    ui_label = "Sharpness";
> = 0.35;

uniform float SampleRadius <
    ui_type = "slider";
    ui_min = 0.25; ui_max = 1.5;
    ui_label = "Spatial sample radius";
> = 0.75;

float3 SampleBackBuffer(float2 texcoord)
{
    return tex2D(ReShade::BackBuffer, texcoord).rgb;
}

float Luma(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

float3 SpatialUpscalePS(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    const float2 px = ReShade::PixelSize * SampleRadius;

    float3 center = SampleBackBuffer(texcoord);
    float3 n = SampleBackBuffer(texcoord + float2(0.0, -px.y));
    float3 s = SampleBackBuffer(texcoord + float2(0.0, px.y));
    float3 e = SampleBackBuffer(texcoord + float2(px.x, 0.0));
    float3 w = SampleBackBuffer(texcoord + float2(-px.x, 0.0));
    float3 ne = SampleBackBuffer(texcoord + float2(px.x, -px.y));
    float3 nw = SampleBackBuffer(texcoord + float2(-px.x, -px.y));
    float3 se = SampleBackBuffer(texcoord + float2(px.x, px.y));
    float3 sw = SampleBackBuffer(texcoord + float2(-px.x, px.y));

    float3 cross_avg = (n + s + e + w) * 0.25;
    float3 diagonal_avg = (ne + nw + se + sw) * 0.25;
    float3 low_pass = center * 0.55 + cross_avg * 0.35 + diagonal_avg * 0.10;

    float3 detail = center - low_pass;
    float edge = abs(Luma(center) - Luma(cross_avg));
    float adaptive = saturate(edge * 6.0);
    float sharpen = lerp(Sharpness, Sharpness * 0.45, adaptive);

    float3 result = center + detail * sharpen * 1.8;
    float3 min_color = min(center, min(min(n, s), min(e, w)));
    float3 max_color = max(center, max(max(n, s), max(e, w)));

    return saturate(clamp(result, min_color - 0.02, max_color + 0.02));
}

technique FSR1SpatialUpscale
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = SpatialUpscalePS;
    }
}
