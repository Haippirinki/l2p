uniform sampler2D u_sampler;

in highp vec2 v_uv;
in lowp vec4 v_colorMul;
in lowp vec3 v_colorAdd;

out highp vec4 fragmentColor;

void main()
{
	lowp vec4 s = texture(u_sampler, v_uv);
	fragmentColor = s * v_colorMul + vec4(v_colorAdd * s.a, 0.0);
}
