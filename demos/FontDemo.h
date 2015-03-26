//
//  FontDemo.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/25/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_FontDemo_h
#define libOOGL_FontDemo_h

#include "GLApplication.h"
#include "GLTextRenderManager.h"
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


class FontDemo : public GLApplication{
public:
    virtual void CreateApplication(){
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        
        GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/HiraginoMaruGothic.otf"));
        
        GLTextRenderManager::AddText(
                                     "line1",
                                     L"Lorem ipsum dolor sit amet, consectetur adipisici elit.",
                                     48, -1 + 8 * sx,
                                     1 - 48 * sy, sx, sy);
        
        GLTextRenderManager::AddText("line2",
                                     L"本当の始まりよ、本当の終わりへ。",
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
        sprintf(buf, "Frame %d", counter++);
        GLTextRenderManager::AddText(NULL, buf, 100, -1 + sx * 50, 1 - 200 * sy, sx, sy,
                                     glm::vec3(1.0f, 0.0f, 0.0f));
        
        GLTextRenderManager::RenderText();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
