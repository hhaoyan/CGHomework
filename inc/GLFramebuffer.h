//
//  GLFramebuffer.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/21/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __libOOGL__GLFramebuffer__
#define __libOOGL__GLFramebuffer__

#include "gl.h"
#include "GLObject.h"
#include "GLAttachable.h"

class GLFramebuffer : public GLObject, public GLAttachable{
public:
    enum FramebufferType{
        kColorOnly = 1,
        kDepthOnly = 2,
        kColorDepth = 1|2,
    };
    GLFramebuffer(int width, int height, GLint magFilter=GL_LINEAR, GLint minFilter=GL_LINEAR, FramebufferType type=kColorDepth);
    ~GLFramebuffer();
    
    void Resize(int width, int height);
    
    virtual bool IsAttached() {return fIsAttached;}
    virtual void Attach();
    virtual void Detach();
    
    void BindFramebuffer(bool clear=true);
    void UnbindFramebuffer();
    
    GLuint GetFramebuffer();
    GLuint GetColorTexture();
    GLuint GetDepthTexture();
    
    void SetColorTextureParameteri(GLint what, GLint how);
    void SetDepthTextureParameteri(GLint what, GLint how);
    
    void ActivateAndBindColorTexture(GLuint slot);
    void ActivateAndBindDepthTexture(GLuint slot);
    
protected:
    virtual void DeleteObject();
    
private:
    FramebufferType fFramebufferType;
    int   fBufferSize[2];
    GLint       fMagfilter, fMinfilter;
    GLuint      fFramebufferObject;
    GLuint      fColorTextureObject, fDepthTextureObject;
    bool        fIsAttached;
};

#endif /* defined(__OpenGLFirst__GLFramebuffer__) */
