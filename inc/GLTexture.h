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
#include "GLAttachable.h"

class GLCubemapTexture;

/**
 * @brief OpenGL 2D texture
 *
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

class GLTexture : public GLObject, public GLAttachable{
public:
    friend class GLCubemapTexture;
    
    /**
     * @brief Enum describing how GLTexture stores texture pixels
     */
    enum TextureMode{
        kBGRA8888 = 0, /**< Four 8 bit color channels ordered in (Blue, Green, Red, Alpha) */
        kRGBA8888, /**< Four 8 bit color channels ordered in (Red, Green, Blue, Alpha) */
        kBGR888, /**< Three 8 bit color channels ordered in (Blue, Green, Red) */
        kRGB888, /**< Three 8 bit color channels ordered in (Red, Green, Blue) */
        kGrayscale8, /**< Simulate grayscale bitmap using red channel */
        
        kINVALID,
    };
    
    /**
     * @brief Load texture from file.
     *
     * This is platform dependent, whether your image format is supported
     * depends on the platform implementation, however most of the formats
     * like jpeg, bmp, png are supported.
     * @param[in] filename File name for texture.
     * @param[in] old If not NULL, then the old GLTexture object will be reused.
     */
    static GLTexture* LoadFromFile(const char* filename, GLTexture* old=NULL);
    
    /**
     * @brief Default constructor
     */
    GLTexture();
    /**
     * @brief Initialize texture with size and mode.
     */
    GLTexture(int width, int height, TextureMode mode);
    ~GLTexture();
    
    /**
     * @brief Set the texture size.
     *
     * After an GLTexture object is created, the size and mode of it
     * must be explicitly specified. Calling this method also clears
     * the buffer, but NOT zeroing it.
     */
    void SetTextureSize(int width, int height, TextureMode mode);
    /**
     * @brief Copy texture pixels.
     *
     * This method fills the image buffer. Note that buf is not 
     * freed by GLTexture, the pixel data is copyed internally, caller 
     * must be responsible for the lifecycle of buf.
     * @param[in] buf The source buffer to copy from.
     * @param[in] bufSize Default value is set to the size computed
     * from image size.
     */
    void SetTextureBuffer(void* buf, int bufSize=-1);
    /**
     * @brief Gets the internal image buffer.
     *
     * This gets the internal image buffer for accessing, the caller must
     * not free or extend it.
     * @param[out] buf
     * @param[out] bufSize
     */
    void GetTextureBuffer(void** buf, int* bufSize);
    
    /**
     * @brief A shortcut for glTextureParameteri.
     */
    void SetTextureParameteri(GLint what, GLint how);
    
    /**
     * @brief A shortcut for glActivateTexture and glBindTexture.
     */
    void ActivateAndBindTexture(GLuint slot);
    
    /**
     * @brief Gets the OpenGL texture object name.
     */
    GLuint GetTextureObject();
    
    /**
     * @brief Lets OpenGL compute mipmap from existing image buffer.
     */
    void GenerateMipmap();
    
    /**
     * @brief Flips the image.
     */
    void FlipBuffer();
    
    int TextureWidth() {return fTextureSize[0];}
    int TextureHeight() {return fTextureSize[1];}
    
    virtual void Attach();
    virtual void Detach();
    virtual bool IsAttached();
    
protected:
    virtual void DeleteObject();
    
private:
    void TexImage2DInternal(GLenum target, GLint level);
    
    bool     fIsAttached;
    GLuint   fTexture;
    void    *fBuffer; // the image buffer in memory, we own
    int      fBufferSize;
    TextureMode fMode;
    int   fTextureSize[2];
};

#endif /* defined(__OpenGLFirst__GLTexture__) */
