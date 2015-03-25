//
//  GLFramebuffer.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/21/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLFramebuffer.h"

GLFramebuffer::GLFramebuffer(int width, int height, GLint magFilter, GLint minFilter, FramebufferType type):
    fMagfilter(magFilter), fMinfilter(minFilter),
    fIsAttached(false), fFramebufferType(type){
    
	// TODO: we can't do this in initializer because a compiler bug of VS2013, 
	// maybe this will be fixed in next generation of VS
	fBufferSize[0] = width;
	fBufferSize[1] = height;
}

GLFramebuffer::~GLFramebuffer(){
    if(IsAttached())
        Warning(__FUNCTION__, "GLFramebuffer at %p is still attached!", this);
}

void GLFramebuffer::Resize(int width, int height){
    fBufferSize[0] = width;
    fBufferSize[1] = height;
}

void GLFramebuffer::Attach(){
    if(IsAttached())
        return;
    
    glGenFramebuffers(1, &fFramebufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, fFramebufferObject);
    
    if(fFramebufferType & kColorOnly){
        glGenTextures(1, &fColorTextureObject);
        
        glBindTexture(GL_TEXTURE_2D, fColorTextureObject);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, fBufferSize[0], fBufferSize[1], 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fMagfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fMinfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fColorTextureObject, 0);
    }
    
    if(fFramebufferType & kDepthOnly){
        glGenTextures(1, &fDepthTextureObject);
        
        glBindTexture(GL_TEXTURE_2D, fDepthTextureObject);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, fBufferSize[0], fBufferSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fMagfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fMinfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fDepthTextureObject, 0);
    }
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Error(__FUNCTION__, "framebuffer creation failed");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    fIsAttached = true;
}

void GLFramebuffer::Detach(){
    if(!IsAttached())
        return;
    
    glDeleteFramebuffers(1, &fFramebufferObject);
    
    if(fFramebufferType & kColorOnly)
        glDeleteTextures(1, &fColorTextureObject);
    if(fFramebufferType & kDepthOnly)
        glDeleteTextures(1, &fDepthTextureObject);
    
    fIsAttached = false;
}

void GLFramebuffer::DeleteObject(){
    if(IsAttached())
        Detach();
}

void GLFramebuffer::SetColorTextureParameteri(GLint what, GLint how){
    if((fFramebufferType & kColorOnly) == 0)
        Error(__FUNCTION__, "GLFramebuffer doesn't have color component");
    
    if(!IsAttached())
        Error(__FUNCTION__, "GLFramebuffer is not attached");
    
    glBindTexture(GL_TEXTURE_2D, fColorTextureObject);
    glTexParameteri(GL_TEXTURE_2D, what, how);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLFramebuffer::SetDepthTextureParameteri(GLint what, GLint how){
    if((fFramebufferType & kDepthOnly) == 0)
        Error(__FUNCTION__, "GLFramebuffer doesn't have depth component");
    
    if(!IsAttached())
        Error(__FUNCTION__, "GLFramebuffer is not attached");
    
    glBindTexture(GL_TEXTURE_2D, fDepthTextureObject);
    glTexParameteri(GL_TEXTURE_2D, what, how);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLFramebuffer::BindFramebuffer(bool clear){
    glBindFramebuffer(GL_FRAMEBUFFER, fFramebufferObject);
    if(clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFramebuffer::UnbindFramebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::ActivateAndBindColorTexture(GLuint slot){
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, GetColorTexture());
}

void GLFramebuffer::ActivateAndBindDepthTexture(GLuint slot){
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, GetDepthTexture());
}

GLuint GLFramebuffer::GetFramebuffer(){
    if(!IsAttached())
        Error(__FUNCTION__, "framebuffer isn't attached!");
    
    return fFramebufferObject;
}

GLuint GLFramebuffer::GetColorTexture(){
    if(!IsAttached())
        Error(__FUNCTION__, "framebuffer isn't attached!");
    
    if((fFramebufferType & kColorOnly) == 0)
        Error(__FUNCTION__, "framebuffer does not have color component");
    
    return fColorTextureObject;
}

GLuint GLFramebuffer::GetDepthTexture(){
    if(!IsAttached())
        Error(__FUNCTION__, "framebuffer isn't attached!");
    
    if((fFramebufferType & kDepthOnly) == 0)
        Error(__FUNCTION__, "framebuffer does not have depth component");
    
    return fDepthTextureObject;
}