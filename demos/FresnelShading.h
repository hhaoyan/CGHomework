//
//  FresnelShading.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/26/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_FresnelShading_h
#define libOOGL_FresnelShading_h

#include "GLApplication.h"
#include "GLTextRenderManager.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "TrackBallCamera.h"
#include "glm/gtc/random.hpp"
#include <unistd.h>
#include "GLSkybox.h"


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

class FresnelShading : public GLApplication{
    GLCubemapTexture* skytex;
    GLSkybox * sky;
    TrackBallCamera* camera;
    GLShader* fshader;
    GLMesh* sphere;
public:
    virtual void MouseScroll(double x, double y){
        if(y > 0)
            camera->ZoomOut();
        else
            camera->ZoomIn();
    }
    virtual void CreateApplication(){
        fshader = new GLShader(AssetManager::GetAsset("fresnel.shader"), "cameraPosition\nmRefractionRatio\nmFresnelBias\nmFresnelScale\nmFresnelPower\nmvpMatrix\nmodelMatrix\ntCube");
        fshader->Attach();
        
        sphere = GLMesh::SimpleMeshGenerator::Sphere(GLMesh::kPosTexNor, 1.0f, 50, 50);
        //sphere = GLMesh::SimpleMeshGenerator::Cuboid(GLMesh::kPosTexNor, 1.0f, 1.0f, 1.0f);
        sphere->Attach();
        
        camera = new TrackBallCamera(this, 5.0f, 0.1f, 100.0f, 45.0);
        
        skytex = GLCubemapTexture::LoadFromFiles(AssetManager::GetAsset("/Park2/"),
                                          "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg");
        skytex->Attach();
        sky = new GLSkybox(skytex);
        sky->Attach();
        
        GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/Consolas.ttf"));
        glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
        
        glEnable(GL_DEPTH_TEST);
        OpenGLShouldHaveNoError(__FUNCTION__);
    }
    
    virtual void RenderFrame(){
        camera->OnFrameUpdate();
        glm::mat4 mvp = camera->GetProjectionViewMatrix();
        glm::vec3 cameraPosition = camera->GetEyePosition();
        glm::mat4 ident = glm::translate(glm::vec3(0, 0, 0));
        
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky->RenderSkybox(mvp, cameraPosition);
        
        fshader->UseProgram();
        
        fshader->Uniform1f("mRefractionRatio", 1.02);
        fshader->Uniform1f("mFresnelBias", 0.1);
        fshader->Uniform1f("mFresnelPower", 2.0);
        fshader->Uniform1f("mFresnelScale", 1.0);
        fshader->Uniform3fv("cameraPosition", 1, &cameraPosition[0]);
        fshader->UniformMatrix4fv("mvpMatrix", 1, GL_FALSE, &mvp[0][0]);
        fshader->UniformMatrix4fv("modelMatrix", 1, GL_FALSE, &ident[0][0]);
        fshader->Uniform1i("tCube", 0);
        
        skytex->ActivateAndBindTexture(GL_TEXTURE0);
        sphere->UseMeshAndDrawTriangles();
        
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        char buf[100];
        static int counter = -1;
        
        sprintf(buf, "Frame: %.4d", ++counter);
        glm::vec3 color(1.0f, 0.7f, 0.9f);
        GLTextRenderManager::AddText(NULL, buf, 50, -1 + sx * 50, 1 - 100 * sy, sx, sy,
                                     color);
        
        
        GLTextRenderManager::RenderText();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
