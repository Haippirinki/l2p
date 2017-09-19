layout(location = 0) in vec2 a_position;

out highp vec2 v_uv;

void main()
{
	v_uv = (a_position + vec2(1.0, 1.0)) * 0.5;
	gl_Position = vec4(a_position, 0.0, 1.0);
}
