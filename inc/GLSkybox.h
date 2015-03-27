//
//  GLSkybox.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/26/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_GLSkybox_h
#define libOOGL_GLSkybox_h

#include "GLCubemapTexture.h"
#include "GLMesh.h"
#include "GLObject.h"
#include "GLShader.h"

/**
 * @brief Renders a cube map as the sky box.
 *
 * Since you must have a cubemap texture ready before creating
 * sky box, there is no need to "Attach" cubemap (auto attached),
 * This behavior may change in the future.
 */
class GLSkybox : public GLObject, public GLAttachable{
public:
    /** 
     * @brief Default constructor, you must have a cubemap initialized.
     */
    GLSkybox(GLCubemapTexture* cubemap);
    ~GLSkybox();
    
    /**
     * @brief Create a skybox from some cubemap textures.
     */
    static GLSkybox* CreateFromCubemap(const char* prefix,
                                       const char* px, const char* nx,
                                       const char* py, const char* ny,
                                       const char* pz, const char* nz);
    
    /**
     * @brief Renders the skybox.
     */
    void RenderSkybox(glm::mat4 mvp, glm::vec3 cameraPosition);
    
    virtual void Attach(){
        /* do nothing, developers should assume GLskybox needs attaching.*/
        fIsAttached = true;
    }
    virtual bool IsAttached(){ return fIsAttached; }
    virtual void Detach();
private:
    bool fIsAttached;
    GLMesh* fSkyGeometry;
    GLCubemapTexture* fCubemap;
    GLShader* fShader;
};

#endif
