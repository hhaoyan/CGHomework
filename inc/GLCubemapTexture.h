//
//  GLCubemapTexture.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/24/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_GLCubemapTexture_h
#define libOOGL_GLCubemapTexture_h

#include "GLTexture.h"
#include "GLObject.h"
#include "GLAttachable.h"

class GLCubemapTexture : protected GLObject, public GLAttachable{
public:
    GLCubemapTexture();
    GLCubemapTexture(int size, GLTexture::TextureMode mode);
    ~GLCubemapTexture();
    enum CubeFaceId{
        kPositiveX = 0,
        kNegativeX,
        kPositiveY,
        kNegativeY,
        kPositiveZ,
        kNegativeZ,
    };
    /**
     * @brief Loads cube map from a list of files
     *
     * @param[in] prefix the prefix of all files
     */
    static GLCubemapTexture* LoadFromFiles(const char* prefix,
                                           const char* px, const char* nx,
                                           const char* py, const char* ny,
                                           const char* pz, const char* nz);
    /**
     * @brief Sets the texture size, and clears the image buffer.
     *
     * This resets the buffer in memory to a new value, but doesn't 
     * reallocate the graphics buffer.
     * @param[in] size The new size of cube map.
     * @param[in] mode The mode for the texture.
     */
    void SetTextureSize(int size, GLTexture::TextureMode mode);
    /**
     * @brief Transforms the data from buf to the internal
     * memory buffer.
     * @see GetTextureBuffer()
     */
    void SetTextureBuffer(CubeFaceId face, void* buf, int bufSize=-1);
    /**
     * @brief Gets the data from internal memory buffer to buf.
     * @see SetTextureBuffer()
     */
    void GetTextureBuffer(CubeFaceId face, void** buf, int* bufSize);
    
    void ActivateAndBindTexture(GLuint slot);
    GLuint GetTextureObject();
    
    /**
     * @brief Gets the texture size.
     */
    int TextureHeight() {return fTextureSize;}
    
    virtual void Attach();
    virtual void Detach();
    virtual bool IsAttached() {return fIsAttached;}
    
protected:
    bool     fIsAttached;
    GLuint   fTexture;
    
    GLTexture fTextures[6]; /**< The six face of cube */
    
    GLTexture::TextureMode fMode;
    int   fTextureSize;
};
#endif
