//
//  GLCubemapTexture.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/24/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLCubemapTexture.h"

GLCubemapTexture::GLCubemapTexture():
fIsAttached(false), fTextureSize(-1){
    
}

GLCubemapTexture::GLCubemapTexture(int size, GLTexture::TextureMode mode):
fIsAttached(false), fTextureSize(-1){
    SetTextureSize(size, mode);
}

GLCubemapTexture::~GLCubemapTexture(){
    
}

GLCubemapTexture* GLCubemapTexture::LoadFromFiles(const char* prefix,
                                       const char* px, const char* nx,
                                       const char* py, const char* ny,
                                       const char* pz, const char* nz){
    GLCubemapTexture* map = new GLCubemapTexture();
    std::string pf=prefix;
    std::string fn;
    int size;
    
    fn = pf + px;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+0))
        goto fail;
    
    fn = pf + nx;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+1))
        goto fail;
    
    fn = pf + py;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+2))
        goto fail;
    
    fn = pf + ny;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+3))
        goto fail;
    
    fn = pf + pz;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+4))
        goto fail;
    
    fn = pf + nz;
    if(!GLTexture::LoadFromFile(fn.c_str(), map->fTextures+5))
        goto fail;
    
    size = map->fTextures[0].TextureWidth();
    for(int i = 0;i<6;++i){
        if(size != map->fTextures[i].TextureWidth()||
           size != map->fTextures[i].TextureHeight()){
            
            map->Warning(__FUNCTION__, "%s:%s,%s,%s,%s,%s,%s is not a valid group of cube map texture",
                    prefix, px, nx, py, ny, pz, nz);
            goto fail;
        }
        map->fTextures[i].FlipBuffer();
    }
    
    map->fTextureSize = size;
    return map;
    
fail:
    delete map;
    return NULL;
}

void GLCubemapTexture::ActivateAndBindTexture(GLuint slot){
    if(!IsAttached())
        Error(__FUNCTION__, "cube map texture not attached");
    
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, fTexture);
}

GLuint GLCubemapTexture::GetTextureObject(){
    if(!IsAttached())
        Error(__FUNCTION__, "cube map texture not attached");
    
    return fTexture;
}

void GLCubemapTexture::SetTextureSize(int size, GLTexture::TextureMode mode){
    for (int i = 0; i<6; ++i) {
        fTextures[i].SetTextureSize(size, size, mode);
    }
    fTextureSize = size;
}

void GLCubemapTexture::SetTextureBuffer(GLCubemapTexture::CubeFaceId face, void *buf, int bufsize){
    fTextures[face].SetTextureBuffer(buf, bufsize);
}

void GLCubemapTexture::GetTextureBuffer(GLCubemapTexture::CubeFaceId face, void **buf, int *bufSize){
    fTextures[face].GetTextureBuffer(buf, bufSize);
}

void GLCubemapTexture::Attach(){
    if(IsAttached())
        return;
    
    glGenTextures(1, &fTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, fTexture);
    
    GLuint mapper[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };
    for(int i = 0;i<6;++i){
        fTextures[i].TexImage2DInternal(mapper[i], 0);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    fIsAttached = true;
}

void GLCubemapTexture::Detach(){
    if(!IsAttached())
        return;
    
    glDeleteTextures(1, &fTexture);
    fIsAttached = false;
}