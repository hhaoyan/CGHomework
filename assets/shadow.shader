// !FragmentShader!
#version 400

uniform sampler2D myTextureSampler;
uniform vec3 v3Eye;
uniform sampler2DShadow depthSampler;
uniform float bias;

in vec3 ex_Normal;
in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec4 shadowCoord;
out vec4 out_Color;

void main(void){


    vec4 shadowCoordxyz = shadowCoord / shadowCoord.w;
    float depthOld = 0;

    shadowCoordxyz.z -= bias;

    int time = 0;
    for(int i = -1;i <= 1 ; i++){
    for(int j = -1 ; j <= 1 ; j++){
            depthOld += texture(depthSampler, shadowCoordxyz.xyz + vec3(1.0f * i / 1024, 1.0f * j / 1024, 0.0f));
        time ++;
    }}

    depthOld /= time;

    vec4 ex_Color = vec4(0.13f, 0.13f, 0.13f, 1.0f);
    if(depthOld > 0.0001f){
        ex_Color += clamp(dot(vec3(-0.577f, 0.577f, 0.577f), ex_Normal), 0, 1) * vec4(0.8f, 0.8f, 0.8f, 1.0f);

        vec3 R = reflect(vec3(-5.0f, 5.0f, 5.0f) - ex_Position, ex_Normal);
        R = normalize(R);
        float cosAlpha = -dot(R, normalize(v3Eye - ex_Position));
        cosAlpha = clamp(cosAlpha, 0, 1);

        ex_Color += vec4(1.0f, 1.0f, 1.0f, 1.0f) * pow(cosAlpha, 8);
        ex_Color *= depthOld;
    }

    out_Color = texture(myTextureSampler, ex_Texcoord) * ex_Color;
    //out_Color = vec4(depthOld, depthOld, depthOld, 1.0f);
}
//!FragmentShaderEnd!

//!VertexShader!

#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;
layout(location=2) in vec3 in_Normal;

out vec3 ex_Normal;
out vec3 ex_Position;
out vec2 ex_Texcoord;
out vec4 shadowCoord;
uniform mat4 mtTrans;
uniform mat4 mtLight;

void main(void)
{
    gl_Position = mtTrans * vec4(in_Position, 1.0f);
    shadowCoord = mtLight * vec4(in_Position, 1.0f);

    ex_Normal = in_Normal;
    ex_Position = in_Position;
    ex_Texcoord = in_Texcoord;

}

//!VertexShaderEnd!