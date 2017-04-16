uniform sampler2D u_sampler;

in highp vec2 v_uv;

out highp vec4 fragmentColor;

void main()
{
	highp float a = texture(u_sampler, v_uv).a;
	fragmentColor = vec4(a, a, a, 1.0);
}
