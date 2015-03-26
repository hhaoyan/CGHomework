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

layout(location=0) out vec3 x1t;
layout(location=1) out vec3 p1t;

in vec2 texcoord;

uniform sampler2D x0t;
uniform sampler2D p0t;

void main(void)
{
    vec4 x0 = texture(x0t, texcoord);
    vec4 p0 = texture(p0t, texcoord);

    p0 += -0.4 * x0 * 0.005;
    x0 += p0 * 0.005;

    x1t = vec3(x0.x, x0.y, x0.z);
    p1t = vec3(p0.x, p0.y, p0.z);
}

//!FragmentShaderEnd!