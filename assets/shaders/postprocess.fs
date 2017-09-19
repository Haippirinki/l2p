layout(std140) uniform Uniforms
{
	highp vec2 u_inverseViewportSize;
};

uniform sampler2D u_sampler;

in highp vec2 v_uv;

out highp vec4 fragmentColor;

void main()
{
	highp vec4 s = texture(u_sampler, v_uv);
	fragmentColor = vec4((s.rgb + dot(s.rgb, vec3(0.333, 0.333, 0.333))) * 0.5, 1.0);
}
