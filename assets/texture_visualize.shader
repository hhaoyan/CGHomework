//!VertexShader!
#version 400

layout(location=0) in vec4 coord;
out vec2 texcoord;

void main(void) {
    gl_Position = vec4(coord.xy, 0, 1);
    texcoord = coord.zw;
}

//!VertexShaderEnd!

//!FragmentShader!
#version 400

layout(location=0) out vec4 color;

in vec2 texcoord;

uniform sampler2D tex;

void main(void)
{
    color = texture(tex, texcoord);
}

//!FragmentShaderEnd!