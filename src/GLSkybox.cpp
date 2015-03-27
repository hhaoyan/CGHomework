//
//  GLSkybox.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/26/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLSkybox.h"

const char SkyBoxShader[] = R"SHADER(
//!Shader!
//!VertexShader!
#version 400
layout(location = 0) in vec3 position;

uniform mat4 MVP;
out vec3 texcoord;

void main(){
    gl_Position = MVP * vec4(position, 1.0f);
    texcoord = position;
}

//!VertexShaderEnd!
//!FragmentShader!
#version 400

in vec3 texcoord;
out vec4 out_color;

uniform samplerCube tex;

void main(){
    out_color = texture(tex, texcoord);
}
//!FragmentShaderEnd!
)SHADER";

GLSkybox::GLSkybox(GLCubemapTexture* cubemap):
fCubemap(cubemap), fIsAttached(false){
    
    fShader = new GLShader(SkyBoxShader, "MVP\ntex");
    fSkyGeometry = GLMesh::SimpleMeshGenerator::Cuboid(GLMesh::kPosition, 1.0f, 1.0f, 1.0f);
    fSkyGeometry->Attach();
}

GLSkybox::~GLSkybox(){
    delete fCubemap;
    delete fShader;
    delete fSkyGeometry;
    if(IsAttached())
        Warning(__FUNCTION__, "GLSkybox isn't detached");
}

void GLSkybox::Detach(){
    fCubemap->Detach();
    fSkyGeometry->Detach();
    fIsAttached = false;
}

void GLSkybox::RenderSkybox(glm::mat4 mvp, glm::vec3 cameraPosition){
    fShader->UseProgram();
    bool depthEnabled = (glIsEnabled(GL_DEPTH_TEST) == GL_TRUE);
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    mvp = mvp * glm::translate(cameraPosition);
    
    fCubemap->ActivateAndBindTexture(GL_TEXTURE0);
    fShader->Uniform1i("tex", 0);
    fShader->UniformMatrix4fv("MVP", 1, GL_FALSE, &mvp[0][0]);
    
    fSkyGeometry->UseMeshAndDrawTriangles();
    if(depthEnabled)
        glEnable(GL_DEPTH_TEST);
    
    if(depthMask == GL_TRUE)
        glDepthMask(GL_TRUE);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
}

GLSkybox* GLSkybox::CreateFromCubemap(const char *prefix, const char *px, const char *nx, const char *py, const char *ny, const char *pz, const char *nz){
    GLCubemapTexture* tex = GLCubemapTexture::LoadFromFiles(prefix, px, nx, py, ny, pz, nz);
    tex->Attach();
    return new GLSkybox(tex);
}
