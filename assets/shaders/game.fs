uniform sampler2D u_sampler;

in highp vec2 v_uv;
in lowp vec4 v_colorMul;
in lowp vec3 v_colorAdd;

out highp vec4 fragmentColor;

void main()
{
	lowp vec4 sample = texture(u_sampler, v_uv);
	fragmentColor = sample * v_colorMul + vec4(v_colorAdd * sample.a, 0.0);
}
