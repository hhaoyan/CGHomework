//
//  GLTexture.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/20/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLTexture.h"
#include <fstream>

GLTexture::GLTexture() :
    fIsAttached(false), fBuffer(0), fTextureSize{0,0}, fTexture(0){
    
}

GLTexture::GLTexture(int width, int height, TextureMode mode):
    fIsAttached(false), fBuffer(0), fTextureSize{0,0}, fTexture(0){
    
    SetTextureSize(width, height, mode);
}

#pragma pack(push,1)
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
struct bmpHeader{
    WORD	bfType;
    DWORD	bFilesize;
    WORD	bfR1;
    WORD	bfR2;
    DWORD	bfOffsetToData;
};

struct bmpInfo{
    DWORD	biSize;
    LONG	biWidth, biHeight;
    WORD	biPlanes;
    WORD	biBitCount;
    DWORD	biCompression;
    DWORD	biSizeImage;
    LONG	ppm[2];
    DWORD	biclrs[2];
};
#pragma pack(pop)

GLTexture* GLTexture::LoadFromBMPFile(const char *filename){
    GLTexture * texture = new GLTexture();
    
    bmpHeader fileHeader;
    bmpInfo fileInfo;
    std::ifstream fin(filename, std::ios::binary);
    if(fin.fail()){
        texture->Warning(__FUNCTION__, "can't open %s for read", filename);
        delete texture;
        return NULL;
    }
    
    fin.read((char*)&fileHeader, sizeof(fileHeader));
    if (fileHeader.bfType != 0x4d42) {
        texture->Warning(__FUNCTION__, "%s is not a bitmap file", filename);
        delete texture;
        return NULL;
    }
    
    fin.read((char*)&fileInfo, sizeof(fileInfo));
    if(fileInfo.biSize != sizeof(fileInfo) ||
       fileInfo.biCompression != 0 ||
       fileInfo.biBitCount != 24){
        texture->Warning(__FUNCTION__, "%s is not a readable bitmap file", filename);
        delete texture;
        return NULL;
    }
    
    texture->SetTextureSize(fileInfo.biWidth, glm::abs(fileInfo.biHeight), kBGR888);
    
    void* buf;
    int bufferSize;
    texture->GetTextureBuffer(&buf, &bufferSize);
    
    if(bufferSize != glm::abs(fileInfo.biHeight) * fileInfo.biWidth * 3){
        texture->Warning(__FUNCTION__, "buffer size mismatch %d:%d", bufferSize,
                         glm::abs(fileInfo.biHeight) * fileInfo.biWidth * 3);
        delete texture;
        return NULL;
    }
    
    char* data = reinterpret_cast<char*>(buf);
    fin.read(data, bufferSize);
    
    if(fin.gcount() != bufferSize){
        texture->Warning(__FUNCTION__, "IO failed for %s", filename);
        delete texture;
        return NULL;
    }
    
    fin.close();
    
    if(fileInfo.biHeight < 0){
        char* tmp = new char[fileInfo.biWidth * 3];
        int width = fileInfo.biWidth;
        int height = fileInfo.biHeight * -1;
        
        for(int i = 0;i<(fileInfo.biHeight * -1) / 2;i++){
            memcpy(tmp, &data[i * width * 3], width * 3);
            memcpy(&data[i * width * 3], &data[(height - 1 - i) * width * 3], width * 3);
            memcpy(&data[(height - i - 1) * width * 3], tmp, width * 3);
        }
        delete [] tmp;
    }
    
    return texture;
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

void GLTexture::Attach(){
    // allocate graphics memory
    if(IsAttached())
        return;
    
    glGenTextures(1, &fTexture);
    glBindTexture(GL_TEXTURE_2D, fTexture);
    
    switch (fMode) {
        case kBGR888:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fTextureSize[0], fTextureSize[1], 0, GL_BGR, GL_UNSIGNED_BYTE, fBuffer);
            break;
            
        case kBGRA8888:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fTextureSize[0], fTextureSize[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, fBuffer);
            
        case kRGB888:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fTextureSize[0], fTextureSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, fBuffer);
            
        case kRGBA8888:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fTextureSize[0], fTextureSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, fBuffer);
            
        default:
            Error(__FUNCTION__, "invalid texture mode");
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    fIsAttached = true;
    
}