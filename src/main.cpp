//
//  main.cpp
//
//  Created by Haoyan Huo on 3/16/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLApplication.h"
#include "TrackBallCamera.h"
#include "GLShader.h"
#include "GLMesh.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"
#include <fstream>
#include <unistd.h>
#define PWD "/Users/huohaoyan/Desktop/libOOGL/OpenGLFirst/"

class App1 : public GLApplication{
    GLMesh *fTeapot, *fPlane, *fSphere;
    TrackBallCamera *fCamera;
    GLShader *fSimpleShader;
    GLShader *fShader;
    GLShader *fDepth;
    GLTexture *fTexture;
    GLFramebuffer *depthFramebuffer;
    float bias = 0.00003;
public:
    virtual void CreateApplication(){
        fTeapot = GLMesh::LoadFromObjFile(PWD"teapot.obj", GLMesh::kPosTexNor);
        fPlane = GLMesh::SimpleMeshGenerator::PlaneZX(GLMesh::kPosTexNor, 1.0f, 1.0f);
        fTeapot->Attach(); fPlane->Attach();
        
        fSphere = GLMesh::SimpleMeshGenerator::Sphere(GLMesh::kPosTexNor, 0.1f, 40, 40);
        fSphere->Attach();
        
        fCamera = new TrackBallCamera(this, 0.5f, 0.1f, 100.0f, 45.0f);
        
        fShader = new GLShader();
        fShader->Compile(PWD"teapot.txt", "mtTrans\nmtLight\nmyTextureSampler\nv3Eye\ndepthSampler\nbias");
        fDepth = new GLShader();
        fDepth->Compile(PWD"depth.txt", "mtTrans");
        fSimpleShader = GLShader::SimpleShaderFactory::SimpleColoredShader();
        
        fTexture = GLTexture::LoadFromBMPFile(PWD"wall.bmp");
        fTexture->Attach();
        fTexture->GenerateMipmap();
        
        depthFramebuffer = new GLFramebuffer(2048, 2048, GL_LINEAR, GL_LINEAR, GLFramebuffer::kDepthOnly);
        depthFramebuffer->Attach();
        depthFramebuffer->SetDepthTextureParameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        depthFramebuffer->SetDepthTextureParameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
    }
    
    virtual void RenderFrame(){
        depthFramebuffer->BindFramebuffer();
        
        glViewport(0, 0, 2048, 2048);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        fDepth->UseProgram();
        
        glm::mat4 mtLight = glm::lookAt(glm::vec3(-5.f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.707f, 0.707f, 0.0f));
        glm::mat4 perspe = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
        mtLight = perspe * mtLight;
        
        fDepth->UniformMatrix4fv("mtTrans", 1, GL_FALSE, &mtLight[0][0]);
        
        fTeapot->UseMeshAndDrawTriangles();
        
        depthFramebuffer->UnbindFramebuffer();
        
        
        glViewport(0, 0, GetWindowWidth(), GetWindowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        fCamera->OnFrameUpdate();
        glm::mat4 mtTrans = fCamera->GetProjectionViewMatrix();
        
        fShader->UseProgram();
        fShader->UniformMatrix4fv("mtTrans", 1, GL_FALSE, &mtTrans[0][0]);
        fShader->Uniform3f("v3Eye", fCamera->GetEyePosition().x, fCamera->GetEyePosition().y, fCamera->GetEyePosition().z);
        fShader->Uniform1f("bias", bias);
        
        glm::mat4 biasr(
                        0.5f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.5f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.5f, 0.0f,
                        0.5f, 0.5f, 0.5f, 1.0f
                        );
        mtLight = biasr * mtLight;
        fShader->UniformMatrix4fv("mtLight", 1, GL_FALSE, &mtLight[0][0]);
        
        if(IsKeyDown('u') == 1)
            bias += 0.000001f;
        if(IsKeyDown('i') == 1)
            bias -= 0.000001f;
        if(IsKeyDown('q'))
            fCamera->ZoomIn();
        if(IsKeyDown('w'))
            fCamera->ZoomOut();
        
        fTexture->ActivateAndBindTexture(GL_TEXTURE0);
        depthFramebuffer->ActivateAndBindDepthTexture(GL_TEXTURE1);
        
        fShader->Uniform1i("myTextureSampler", 0);
        fShader->Uniform1i("depthSampler", 1);
        
        fTeapot->UseMeshAndDrawTriangles();
        fPlane->UseMeshAndDrawTriangles();
        usleep(25000);
        
        fSimpleShader->UseProgram();
        fSimpleShader->Uniform4f("staticColor", 1.0f, 0.0f, 0.0f, 1.0f);
        mtTrans *= glm::translate(glm::vec3(1.0f, 1.0f, 1.0f));
        fSimpleShader->UniformMatrix4fv("transformMatrix", 1, GL_FALSE, &mtTrans[0][0]);
        fSphere->UseMeshAndDrawTriangles();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

int main(int argc, char** argv){
    GLApplication * app = new App1();
    app->CreateWindow("Demo", 800, 600);
    app->RunMainLoop();
    delete app;
    
    return 0;
}
