layout(std140) uniform Uniforms
{
	mat4 u_modelViewProjection;
};

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_colorMul;
layout(location = 3) in vec3 a_colorAdd;

out highp vec2 v_uv;
out lowp vec4 v_colorMul;
out lowp vec3 v_colorAdd;

void main()
{
	v_uv = a_uv;
	v_colorMul = a_colorMul;
	v_colorAdd = a_colorAdd;
	gl_Position = u_modelViewProjection * vec4(a_position, 0.0, 1.0);
}
