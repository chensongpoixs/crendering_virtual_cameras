#version 330 core
// 这些着色器假设顶点着色器中有一个顶点属性 aPos 表示顶点位置，有一个顶点属性 aTexCoord 表示纹理坐标。片段着色器中使用了一个 uniform 变量 hdrTexture 表示HDR纹理，以及一个 uniform 变量 exposure 表示曝光值。在实际应用中，你需要根据你的场景和需求进行适当的调整和修改。
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D hdrTexture;
uniform float exposure;

// 定义一个可调节的曝光函数
vec3 toneMapping(vec3 value, float exposure) 
{
    return 1.0 - exp(-value * exposure);
}

void main() 
{
    // 从HDR纹理中采样颜色值
    vec3 hdrColor = texture(hdrTexture, TexCoords).rgb;

    // 对颜色值进行曝光调整
    hdrColor = toneMapping(hdrColor, exposure);

    // 将调整后的颜色写入输出
    FragColor = vec4(hdrColor, 1.0);
}

////////////////////////////////////////////////////////////////////////////

 
//#include "ReShadeUI.fxh"

//uniform float HDRPower < __UNIFORM_SLIDER_FLOAT1
//	ui_min = 0.0; ui_max = 8.0;
//	ui_label = "Power";
//> = 1.30;
//uniform float radius1 < __UNIFORM_SLIDER_FLOAT1
//	ui_min = 0.0; ui_max = 8.0;
//	ui_label = "Radius 1";
//> = 0.793;
//uniform float radius2 < __UNIFORM_SLIDER_FLOAT1
//	ui_min = 0.0; ui_max = 8.0;
//	ui_label = "Radius 2";
//	ui_tooltip = "Raising this seems to make the effect stronger and also brighter.";
//> = 0.87;

//#include "ReShade.fxh"

//float3 HDRPass(float4 vpos : SV_Position, float2 texcoord : TexCoord) : SV_Target
//{
//	float3 color = tex2D(ReShade::BackBuffer, texcoord).rgb;
//
//	float3 bloom_sum1 = tex2D(ReShade::BackBuffer, texcoord + float2(1.5, -1.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2(-1.5, -1.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2( 1.5,  1.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2(-1.5,  1.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2( 0.0, -2.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2( 0.0,  2.5) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2(-2.5,  0.0) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum1 += tex2D(ReShade::BackBuffer, texcoord + float2( 2.5,  0.0) * radius1 * BUFFER_PIXEL_SIZE).rgb;
//
//	bloom_sum1 *= 0.005;
//
//	float3 bloom_sum2 = tex2D(ReShade::BackBuffer, texcoord + float2(1.5, -1.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2(-1.5, -1.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2( 1.5,  1.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2(-1.5,  1.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2( 0.0, -2.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;	
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2( 0.0,  2.5) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2(-2.5,  0.0) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//	bloom_sum2 += tex2D(ReShade::BackBuffer, texcoord + float2( 2.5,  0.0) * radius2 * BUFFER_PIXEL_SIZE).rgb;
//
//	bloom_sum2 *= 0.010;
//
//	float dist = radius2 - radius1;
//	float3 HDR = (color + (bloom_sum2 - bloom_sum1)) * dist;
//	float3 blend = HDR + color;
//	color = pow(abs(blend), abs(HDRPower)) + HDR; // pow - don't use fractions for HDRpower
//	
//	return saturate(color);
//}
//
//technique HDR
//{
//	pass
//	{
//		VertexShader = PostProcessVS;
//		PixelShader = HDRPass;
//	}
//}




















