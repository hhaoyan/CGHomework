//
//  VSCompute.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/25/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_VSCompute_h
#define libOOGL_VSCompute_h

#include "GLApplication.h"
#include "GLTextRenderManager.h"
#include "GLTexture.h"
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

/**
 * @brief A class which simulates 64 * 64 particles
 */
class VSCompute : public GLApplication{
    GLTexture *fcoord1, *fcoord2, *fmomentum1, *fmomentum2;
    GLuint fFramebufferObject;
public:
    void ClearBuffer(GLTexture* tex){
        void* buf;
        int bufSize;
        tex->GetTextureBuffer(&buf, &bufSize);
        
        char* rep = reinterpret_cast<char*>(buf);
        for (int i = 0; i<bufSize; ++i) {
            rep[i] = 0;
        }
    }
    
    void SetDefaultXP(GLTexture* x, GLTexture* p){
        {
            // sets the x
            void* buf;
            int bufSize;
            x->GetTextureBuffer(&buf, &bufSize);
            
            float* zero = reinterpret_cast<float*>(buf);
            
            for (int i = 0; i<x->GetTextureHeight(); ++i) {
                for (int j = 0; j<x->GetTextureWidth(); ++j) {
                    float* coord = zero + i * x->GetTextureWidth() * 3 +
                        j * 3;
                    
                    coord[0] = glm::
                }
            }
        }
    }
    
    virtual void SetupComputeTextures(){
        fcoord1 = new GLTexture();
        fcoord2 = new GLTexture();
        fmomentum1 = new GLTexture();
        fmomentum2 = new GLTexture();
        
        fcoord1->SetTextureSize(64, 64, GLTexture::kRGB888);
        fcoord2->SetTextureSize(64, 64, GLTexture::kRGB888);
        fmomentum1->SetTextureSize(64, 64, GLTexture::kRGB888);
        fmomentum2->SetTextureSize(64, 64, GLTexture::kRGB888);
        ClearBuffer(fcoord1); ClearBuffer(fcoord2);
        ClearBuffer(fmomentum1); ClearBuffer(fmomentum2);
        
        fcoord1->Attach(); fcoord2->Attach();
        fmomentum1->Attach(); fmomentum2->Attach();
        
        glGenFramebuffers(1, &fFramebufferObject);
        glBindFramebuffer(GL_FRAMEBUFFER, fFramebufferObject);
    }
    
    virtual void CreateApplication(){
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        
        GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/FangZhengLanTingHei.ttf"));
        
        GLTextRenderManager::AddText(
                                     "line1",
                                     L"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
                                     48, -1 + 8 * sx,
                                     1 - 48 * sy, sx, sy);
        
        GLTextRenderManager::AddText("line2",
                                     L"汉体书写信息技术标准相容\n档案下载使用界面简单",
                                     48,
                                     -1 + 16 * sx, 1 - 100.5 * sy, sx, sy);
        
        OpenGLShouldHaveNoError(__FUNCTION__);
        
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
    }
    
    virtual void RenderFrame(){
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        char buf[100];
        static int counter;
        sprintf(buf, "%d", counter++);
        GLTextRenderManager::AddText(NULL, buf, 100, -1 + sx * 50, 1 - 200 * sy, sx, sy,
                                     glm::vec3(1.0f, 0.0f, 0.0f));
        
        GLTextRenderManager::RenderText();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
