//!VertexShader!
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_Texcoord;

uniform mat4 mtTrans;

void main(void)
{
gl_Position = mtTrans * vec4(in_Position, 1.0f);
}

//!VertexShaderEnd!
//!FragmentShader!
#version 400

layout(location=0) out float depth;

void main(void)
{
depth = gl_FragCoord.z;
}

//!FragmentShaderEnd!