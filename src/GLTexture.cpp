//
//  GLTexture.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/20/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLTexture.h"
#include <fstream>
#include <algorithm>
#include "ObjectiveCInterface.h"

GLTexture::GLTexture() :
    fIsAttached(false), fBuffer(0), fTexture(0){
    
	// TODO: we can't do this in initializer because a compiler bug of VS2013, 
	// maybe this will be fixed in next generation of VS
	fTextureSize[0] = fTextureSize[1] = 0;
}

GLTexture::GLTexture(int width, int height, TextureMode mode):
    fIsAttached(false), fBuffer(0), fTexture(0){
    
	// TODO: we can't do this in initializer because a compiler bug of VS2013, 
	// maybe this will be fixed in next generation of VS
	fTextureSize[0] = fTextureSize[1] = 0;
    SetTextureSize(width, height, mode);
}

void GLTexture::FlipBuffer(){
    if(!fBuffer)
        Error(__FUNCTION__, "no buffer");
    
    int sizePerRow = fBufferSize / TextureHeight();
    char* tmp = new char[sizePerRow];
    
    char* parsed = reinterpret_cast<char*>(fBuffer);
    long height = TextureHeight();

    for(int i = 0;i<height / 2;i++){
        memcpy(tmp, &parsed[i * sizePerRow], sizePerRow);
        memcpy(&parsed[i * sizePerRow], &parsed[(height - 1 - i) * sizePerRow], sizePerRow);
        memcpy(&parsed[(height - i - 1) * sizePerRow], tmp, sizePerRow);
    }
        
    delete [] tmp;
}

GLTexture* GLTexture::LoadFromFile(const char *filename, GLTexture* old){
    GLTexture * texture;
    if(old){
        if(old->IsAttached())
            old->Detach();
        texture = old;
    }else
        texture = new GLTexture();
    
    void* data;
    long width, height, bytesPerPixel;
    
    if(!DecodeImageFile(filename, &data, &width, &height, &bytesPerPixel)){
        texture->Warning(__FUNCTION__, "can't open %s as texture", filename);
        goto load_fail;
    }
    
	if (bytesPerPixel == 32){
		texture->SetTextureSize(static_cast<int>(width), static_cast<int>(height), kRGBA8888);
	}else if (bytesPerPixel == 24){
		texture->SetTextureSize(static_cast<int>(width), static_cast<int>(height), kBGR888);
	}else{
        texture->Warning(__FUNCTION__, "incomplete internal API");
        goto load_fail;
    }
    
    texture->SetTextureBuffer(data);
    FreeDecodedImage(&data);
    return texture;
    
load_fail:
    if(!old)
        delete texture;
    return NULL;
}

void GLTexture::SetTextureSize(int width, int height, TextureMode mode){
    if(fBuffer != NULL){
        free(fBuffer);
        fBuffer = NULL;
    }
    
    fTextureSize[0] = width;
    fTextureSize[1] = height;
    if(((width-1) & width) ||
       ((height-1) & height))
        Warning(__FUNCTION__, "width(%d), height(%d), won't be byte aligned", width, height);

    int ModeToPixelSize[GLTexture::kINVALID] = {
        4, //kBGRA8888
        4, //kRGBA8888
        3, //kBGR888
        3, //kRGB888
        1, //kGrayscale8
    };
    
    fBufferSize = ModeToPixelSize[mode] * width * height;
    fMode = mode;
    
    fBuffer = malloc(fBufferSize);
    if(!fBuffer)
        Error(__FUNCTION__, "allocate memory for texture buffer failed");
}

GLTexture::~GLTexture(){
    if(fBuffer){
        // TODO: thread safety here!
        free(fBuffer);
        fBuffer = 0;
    }
}

void GLTexture::GetTextureBuffer(void **buf, int *bufSize){
    if(!fBuffer)
        Error(__FUNCTION__, "GLTexture not ready");
    
    if(buf){
        *buf = fBuffer;
    }else{
        Warning(__FUNCTION__, "why don't you give me a valid pointer?");
    }
    
    if(bufSize)
        *bufSize = fBufferSize;
}

void GLTexture::ActivateAndBindTexture(GLuint slot){
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, GetTextureObject());
}

void GLTexture::SetTextureBuffer(void *buf, int bufSize){
    if(!fBuffer)
        Error(__FUNCTION__, "GLTexture not ready");
    
    if(!buf)
        Error(__FUNCTION__, "why don't you give me a valid pointer?");
    
    int byteToCopy;
    if(bufSize == -1){
        byteToCopy = fBufferSize;
    } else {
        byteToCopy = std::min(fBufferSize, bufSize);
    }
    
    memcpy(fBuffer, buf, byteToCopy);
}

void GLTexture::DeleteObject(){
    if(IsAttached())
        Detach();
}

bool GLTexture::IsAttached(){
    return fIsAttached;
}

void GLTexture::SetTextureParameteri(GLint what, GLint how){
    if(!IsAttached())
        Error(__FUNCTION__, "trying to set texture parameter to an unattached texture");
    
    glBindTexture(GL_TEXTURE_2D, fTexture);
    glTexParameteri(GL_TEXTURE_2D, what, how);
    OpenGLShouldHaveNoError(__FUNCTION__);
}

GLuint GLTexture::GetTextureObject(){
    if(!IsAttached())
        Error(__FUNCTION__, "GLTexture isn't attached");
    
    return fTexture;
}

void GLTexture::Detach(){
    if(!IsAttached())
        return;
    
    glDeleteTextures(1, &fTexture);
    fIsAttached = false;
}

void GLTexture::GenerateMipmap(){
    if(!IsAttached())
        Error(__FUNCTION__, "can't generate mipmap on an unattached texture");
    
    glBindTexture(GL_TEXTURE_2D, GetTextureObject());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    OpenGLShouldHaveNoError(__FUNCTION__);
}

void GLTexture::TexImage2DInternal(GLenum target, GLint level){
    if(!fBuffer)
        Error(__FUNCTION__, "texture buffer not ready");
    
    switch (fMode) {
        case kBGR888:
            glTexImage2D(target, level, GL_RGB8, fTextureSize[0], fTextureSize[1], 0, GL_BGR, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        case kBGRA8888:
            glTexImage2D(target, level, GL_RGBA8, fTextureSize[0], fTextureSize[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        case kRGB888:
            glTexImage2D(target, level, GL_RGB8, fTextureSize[0], fTextureSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        case kRGBA8888:
            glTexImage2D(target, level, GL_RGBA8, fTextureSize[0], fTextureSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        case kGrayscale8:
            glTexImage2D(target, level, GL_RED, fTextureSize[0], fTextureSize[1], 0, GL_RED, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        default:
            Error(__FUNCTION__, "invalid texture mode");
    }
}

void GLTexture::Attach(){
    // allocate graphics memory
    if(IsAttached())
        return;
    
    glGenTextures(1, &fTexture);
    glBindTexture(GL_TEXTURE_2D, fTexture);
    
    TexImage2DInternal(GL_TEXTURE_2D, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    fIsAttached = true;
    
}