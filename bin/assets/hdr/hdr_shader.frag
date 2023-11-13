#version 330 core
// 这些着色器假设顶点着色器中有一个顶点属性 aPos 表示顶点位置，有一个顶点属性 aTexCoord 表示纹理坐标。片段着色器中使用了一个 uniform 变量 hdrTexture 表示HDR纹理，以及一个 uniform 变量 exposure 表示曝光值。在实际应用中，你需要根据你的场景和需求进行适当的调整和修改。
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D hdrTexture;
uniform float exposure;

// 定义一个可调节的曝光函数
float toneMapping(float value, float exposure) {
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