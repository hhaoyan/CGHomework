//
//  GLTexture.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/20/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __libOOGL__GLTexture__
#define __libOOGL__GLTexture__

#include "gl.h"
#include "GLObject.h"

class GLCubemapTexture;

/*
 * GLTexture is a lazy object. When a GLTexture object is created,
 * there is no buffer in memory neither in graphics memory, to create
 * buffer in memory for R/W access, you must specify the width and 
 * height.
 * A GLTexture with memory buffer but no graphics buffer is said to
 * be in "Detached" state, to allocate the graphics memory, call
 * Attach(). After that, it could be used for drawing in shaders.
 * Note: when object releases, it only frees the memory buffer, be sure
 * to call "Detach" if you want to manually delete before it's automatically
 * recycled by GLApplication::ShutdownApplication.
 */

class GLTexture : public GLObject{
public:
    friend class GLCubemapTexture;
    
    enum TextureMode{
        kBGRA8888 = 0,
        kRGBA8888,
        kBGR888,
        kRGB888,
        kGrayscale8, /**< Simulate grayscale bitmap using red channel */
        
        kINVALID,
    };
    
    static GLTexture* LoadFromBMPFile(const char* filename, GLTexture* old=NULL);
    
    GLTexture();
    GLTexture(int width, int height, TextureMode mode);
    ~GLTexture();
    
    void SetTextureSize(int width, int height, TextureMode mode); // this also clears the buffer
    void SetTextureBuffer(void* buf, int bufSize=-1);
    void GetTextureBuffer(void** buf, int* bufSize);
    
    void SetTextureParameteri(GLint what, GLint how);
    
    void ActivateAndBindTexture(GLuint slot);
    GLuint GetTextureObject();
    
    void GenerateMipmap();
    
    int TextureWidth() {return fTextureSize[0];}
    int TextureHeight() {return fTextureSize[1];}
    
    void Attach();
    void Detach();
    bool IsAttached();
    
protected:
    virtual void DeleteObject();
    
private:
    void glTexImage2DInternal(GLenum target, GLint level);
    
    bool     fIsAttached;
    GLuint   fTexture;
    void    *fBuffer; // the image buffer in memory, we own
    int      fBufferSize;
    TextureMode fMode;
    int   fTextureSize[2];
};

#endif /* defined(__OpenGLFirst__GLTexture__) */
