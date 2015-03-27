//!VertexShader!
#version 400

layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;

uniform float mRefractionRatio;
uniform float mFresnelBias;
uniform float mFresnelScale;
uniform float mFresnelPower;
uniform vec3 cameraPosition;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

out vec3 vReflect;
out vec3 vRefract[3];
out float vReflectionFactor;

void main() {
    vec4 worldPosition = modelMatrix * vec4(position.xyz, 1.0);

    vec3 worldNormal = normalize( mat3( modelMatrix[0].xyz, modelMatrix[1].xyz, modelMatrix[2].xyz ) * normal );

    vec3 I = worldPosition.xyz - cameraPosition;

    vReflect = reflect(I , worldNormal);
    vRefract[0] = refract(normalize(I), worldNormal, mRefractionRatio);
    vRefract[1] = refract(normalize(I), worldNormal, mRefractionRatio * 0.99);
    vRefract[2] = refract(normalize(I), worldNormal, mRefractionRatio * 0.98);
    vReflectionFactor = mFresnelBias + mFresnelScale * pow( 1.0 + dot( normalize( I ), worldNormal ) , mFresnelPower );

    gl_Position = mvpMatrix * vec4( position.xyz,  1.0 );

}

//!VertexShaderEnd!

//!FragmentShader!
#version 400

uniform samplerCube tCube;

in vec3 vReflect;
in vec3 vRefract[3];
in float vReflectionFactor;
out vec4 out_Color;

void main() {
    vec4 reflectedColor = texture(tCube, vec3(vReflect));
    vec4 refractedColor = vec4(1.0);

    refractedColor.r = texture(tCube, vec3(vRefract[0])).r;
    refractedColor.g = texture(tCube, vec3(vRefract[1])).g;
    refractedColor.b = texture(tCube, vec3(vRefract[2])).b;

    // only reflect, this is fine.
    //out_Color = reflectedColor;

    // only refraction, this is NOT fine.
    // TODO: why the color breaks when incident angle are pretty small?
    //out_Color = refractedColor;

    // reasonable, this is NOT fine.
    out_Color = mix( refractedColor, reflectedColor, clamp( vReflectionFactor, 0.0, 1.0 ) );
}

//!FragmentShaderEnd!