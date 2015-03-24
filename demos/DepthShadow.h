//
//  DepthShadow.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/24/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_DepthShadow_h
#define libOOGL_DepthShadow_h

#include "GLApplication.h"
#include "TrackBallCamera.h"
#include "GLShader.h"
#include "GLMesh.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"
#include "GLCubemapTexture.h"
#include <unistd.h>

class AssetManager{
public:
    /**
     * @brief Get the filename of an asset.
     *
     * To make sure your executable loads the assets correctly,
     * you should change the working directory of main executable
     * to the parent directory of "asset" folder.
     *
     * On windows, getcwd should be replaced by GetCurrentDirectory()
     */
    static const char* GetAsset(const char* name){
        static std::string oldAsset;
        
        oldAsset = getcwd(NULL, 0);
        oldAsset.append("/assets/");
        oldAsset.append(name);
        return oldAsset.c_str();
    }
};

class DepthShadowDemo : public GLApplication{
    GLMesh *fTeapot, *fPlane, *fSphere;
    TrackBallCamera *fCamera;
    GLShader *fSimpleShader;
    GLShader *fShader;
    GLShader *fDepth;
    GLTexture *fTexture;
    GLFramebuffer *depthFramebuffer;
    GLCubemapTexture* cubemap;
    float bias = 0.00003;
    
public:
    virtual void MouseScroll(double x, double y){
        bool up = y > 0;
        if(up)
            fCamera->Zoom(0.8);
        else
            fCamera->Zoom(1.1);
    }
    virtual void CreateApplication(){
        cubemap = GLCubemapTexture::LoadFromFiles(AssetManager::GetAsset("Yokohama3/"),
                                                  "posx.bmp",
                                                  "negx.bmp",
                                                  "posy.bmp",
                                                  "negy.bmp",
                                                  "posz.bmp",
                                                  "negz.bmp");
        cubemap->Attach();
        
        glEnable(GL_DEPTH_TEST);
        
        fTeapot = GLMesh::LoadFromObjFile(AssetManager::GetAsset("teapot.obj"), GLMesh::kPosTexNor);
        fPlane = GLMesh::SimpleMeshGenerator::PlaneZX(GLMesh::kPosTexNor, 1.0f, 1.0f);
        fTeapot->Attach(); fPlane->Attach();
        
        fSphere = GLMesh::SimpleMeshGenerator::Sphere(GLMesh::kPosTexNor, 0.1f, 40, 40);
        fSphere->Attach();
        
        fCamera = new TrackBallCamera(this, 0.5f, 0.1f, 100.0f, 45.0f);
        
        fShader = new GLShader();
        fShader->Compile(AssetManager::GetAsset("shadow.shader"), "mtTrans\nmtLight\nmyTextureSampler\nv3Eye\ndepthSampler\nbias");
        fDepth = new GLShader();
        fDepth->Compile(AssetManager::GetAsset("depth.shader"), "mtTrans");
        fSimpleShader = GLShader::SimpleShaderFactory::SimpleColoredShader();
        
        fTexture = GLTexture::LoadFromBMPFile(AssetManager::GetAsset("wall.bmp"));
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
        
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
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
        //usleep(25000);
        
        fSimpleShader->UseProgram();
        fSimpleShader->Uniform4f("staticColor", 1.0f, 0.0f, 0.0f, 1.0f);
        mtTrans *= glm::translate(glm::vec3(1.0f, 1.0f, 1.0f));
        fSimpleShader->UniformMatrix4fv("transformMatrix", 1, GL_FALSE, &mtTrans[0][0]);
        fSphere->UseMeshAndDrawTriangles();
        
        cubemap->ActivateAndBindTexture(GL_TEXTURE4);
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
