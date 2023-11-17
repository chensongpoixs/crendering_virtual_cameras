

 // 传入参数
varying vec3 m_color;
varying vec2 m_texcoord;

uniform sampler2D smp1;
uniform sampler2D smp2;
void main()
{
	//vec3 rgb = texture2D(smp1, m_texcoord).rgb;
	//gl_FragColor = vec4(rgb + m_color * 0.01f, 1.0);
	gl_FragColor = mix(texture2D(smp1, m_texcoord), texture2D(smp2, m_texcoord), 0.5f) * 0.001 + vec4( m_color * 0.001f, 1.0f);
}
