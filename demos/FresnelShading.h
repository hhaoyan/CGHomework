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
    GLSkybox * sky;
    TrackBallCamera* camera;
    GLShader* fshader;
    GLMesh* cube;
    GLTexture * tex;
public:
    virtual void CreateApplication(){
        tex = GLTexture::LoadFromFile(AssetManager::GetAsset("texture_mapping.jpg"));
        tex->Attach();
        
        cube = GLMesh::SimpleMeshGenerator::Cuboid(GLMesh::kPosTexNor, 1.0f, 1.0f, 1.0f);
        cube->Attach();
        fshader = GLShader::SimpleShaderFactory::SimpleTexturedShader();
        
        camera = new TrackBallCamera(this, 5.0f, 0.1f, 100.0f, 45.0);
        sky = GLSkybox::CreateFromCubemap(AssetManager::GetAsset("/Yokohama3/"),
                                          "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg");
        
        GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/Consolas.ttf"));
        glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
        
        glEnable(GL_DEPTH_TEST);
        OpenGLShouldHaveNoError(__FUNCTION__);
    }
    
    virtual void RenderFrame(){
        camera->OnFrameUpdate();
        glm::mat4 mvp = camera->GetProjectionViewMatrix();
        glm::vec3 cameraPosition = camera->GetEyePosition();
        
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        tex->ActivateAndBindTexture(GL_TEXTURE0);
        fshader->UseProgram();
        fshader->Uniform1i("tex", 0);
        fshader->UniformMatrix4fv("transformMatrix", 1, GL_FALSE, &mvp[0][0]);
        cube->UseMeshAndDrawTriangles();
        sky->RenderSkybox(mvp, cameraPosition);
        
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
