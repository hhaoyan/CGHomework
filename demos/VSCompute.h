//
//  VSCompute.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/25/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_VSCompute_h
#define libOOGL_VSCompute_h

#include "GLApplication.h"
#include "GLTextRenderManager.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "glm/gtc/random.hpp"
#include "AssetManager.h"

/**
 * @brief A class which simulates NumOfParticle * NumOfParticle particles
 */
template<int NumOfParticle>
class Computer : GLObject{
    GLuint fVertexBufferObject, fVertexArrayObject;
    GLuint fTextureX[2], fTextureP[2];
    GLuint fFramebufferObject;
    GLShader* fComputeShader;
    GLShader* fVisualizerShader;
    float* fTextureBuffer;
public:
    void Init(){
        {
            // for retriving data from texture
            fTextureBuffer = new float[NumOfParticle * NumOfParticle * 3];
        }
        
        {
            // allocate and fill the texture
            glGenTextures(2, fTextureX);
            glGenTextures(2, fTextureP);
            
            int width = NumOfParticle;
            int height = NumOfParticle;
            int bufSize = width * height * 3 * 4;
            
            char* emptyBuffer = new char[bufSize];
            memset(emptyBuffer, 0, bufSize);
            
            // clear the n+1 texture
            glActiveTexture(GL_TEXTURE0);
            
            glBindTexture(GL_TEXTURE_2D, fTextureX[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, emptyBuffer);
            // DO NOT do any interpolate between pixels(datas)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glBindTexture(GL_TEXTURE_2D, fTextureP[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, emptyBuffer);
            // DO NOT do any interpolate between pixels(datas)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // we set the initial speed to 0
            glBindTexture(GL_TEXTURE_2D, fTextureP[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, emptyBuffer);
            // DO NOT do any interpolate between pixels(datas)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // we set the initial displacement to random
            {
                float * beginning = reinterpret_cast<float*>(emptyBuffer);
                for (int i = 0; i<height; ++i) {
                    for (int j = 0; j<width; ++j) {
                        float *displacement =
                            beginning + i * width * 3 + j * 3;
                        
                        glm::vec3 random = glm::ballRand(1.0f);
                        displacement[0] = random.x;
                        displacement[1] = random.y;
                        displacement[2] = random.z;
                    }
                }
            }
            glBindTexture(GL_TEXTURE_2D, fTextureX[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, emptyBuffer);
            // DO NOT do any interpolate between pixels(datas)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glBindTexture(GL_TEXTURE_2D, 0);
            delete[] emptyBuffer;
            
            OpenGLShouldHaveNoError("ComputeTextureLoading");
        }
        
        {
            // init the framebuffer
            glGenFramebuffers(1, &fFramebufferObject);
            // but dont bind it
            
            OpenGLShouldHaveNoError("ComputeFramebufferLoading");
        }
        
        {
            // init the shader
            fComputeShader = new GLShader(AssetManager::GetAsset("compute.shader"), "x0t\np0t");
            fComputeShader->Attach();
            
            OpenGLShouldHaveNoError("ComputeShaderLoading");
        }
        
        {
            // init the geometry for rendering
            glGenBuffers(1, &fVertexBufferObject);
            glBindBuffer(GL_ARRAY_BUFFER, fVertexBufferObject);
            
            GLfloat box[4][4] = {
                {-1, 1, 0, 1}, // left top
                {-1,-1, 0, 0}, // left down
                { 1, 1, 1, 1}, // right top
                { 1,-1, 1, 0}, // right down
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
            
            glGenVertexArrays(1, &fVertexArrayObject);
            glBindVertexArray(fVertexArrayObject);
            
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            
            OpenGLShouldHaveNoError("ComputeGeometryLoading");
        }
        
        {
            // the visualizer
            fVisualizerShader = new GLShader(AssetManager::GetAsset("texture_visualize.shader"), "tex");
            fVisualizerShader->Attach();
        }
    }
    
    void SelectTextureAndBindToFramebuffer(){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fTextureX[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fTextureP[0]);
        
        fComputeShader->Uniform1i("x0t", 0);
        fComputeShader->Uniform1i("p0t", 1);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fFramebufferObject);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fTextureX[1], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fTextureP[1], 0);
        
        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, drawBuffers);
        
        {
            // and swap textures
            int swap;
            swap = fTextureX[0]; fTextureX[0] = fTextureX[1]; fTextureX[1] = swap;
            swap = fTextureP[0]; fTextureP[0] = fTextureP[1]; fTextureP[1] = swap;
        }
        
        OpenGLShouldHaveNoError("ComputeBindEssential");
    }
    
    void Compute(glm::vec3& x_result, glm::vec3& p_result){
        OpenGLShouldHaveNoError("StartCompute");
        
        glViewport(0, 0, NumOfParticle, NumOfParticle);
        fComputeShader->UseProgram();
        
        {
            // load the geometry
            glBindBuffer(GL_ARRAY_BUFFER, fVertexBufferObject);
            glBindVertexArray(fVertexArrayObject);
        }
        
        SelectTextureAndBindToFramebuffer();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLShouldHaveNoError("DoCompute");
        
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        // performance warning: glGetTexImage is extremely slow, this is only
        // for demonstration and should be avoided in production.
        glActiveTexture(GL_TEXTURE0);
        // tricky here, texture of index 0 is the output of current frame!
        glBindTexture(GL_TEXTURE_2D, fTextureX[0]);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, fTextureBuffer);
        OpenGLShouldHaveNoError("GetComputeResultX");
        x_result.x = fTextureBuffer[0];
        x_result.y = fTextureBuffer[1];
        x_result.z = fTextureBuffer[2];
        
        glBindTexture(GL_TEXTURE_2D, fTextureP[0]);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, fTextureBuffer);
        OpenGLShouldHaveNoError("GetComputeResultP");
        p_result.x = fTextureBuffer[0];
        p_result.y = fTextureBuffer[1];
        p_result.z = fTextureBuffer[2];
        
    }
    
    /**
     * @brief This visualizes the displacement/momentum textures.
     */
    void Visualize(int width, int height){
        int margin = (width + height) / 50;
        int w = width / 2 - 2 * margin;
        int h = height / 2 - 2 * margin;
        
        fVisualizerShader->UseProgram();
        glBindBuffer(GL_ARRAY_BUFFER, fVertexBufferObject);
        glBindVertexArray(fVertexArrayObject);
        
        glActiveTexture(GL_TEXTURE0);
        
        glViewport(0 + margin, height / 2 + margin, w, h);
        glBindTexture(GL_TEXTURE_2D, fTextureX[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLShouldHaveNoError("VisualizerX0");
        
        glViewport(width / 2 + margin, height / 2 + margin, w, h);
        glBindTexture(GL_TEXTURE_2D, fTextureX[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLShouldHaveNoError("VisualizerX1");
        
        glViewport(0 + margin, 0 + margin, w, h);
        glBindTexture(GL_TEXTURE_2D, fTextureP[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLShouldHaveNoError("VisualizerP0");
        
        glViewport(width / 2 + margin, 0 + margin, w, h);
        glBindTexture(GL_TEXTURE_2D, fTextureP[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLShouldHaveNoError("VisualizerP1");
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

class VSCompute : public GLApplication{
    Computer<128> computer;
public:
    virtual void CreateApplication(){
        computer.Init();
        
        GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/Consolas.ttf"));
        glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
        
        OpenGLShouldHaveNoError(__FUNCTION__);
    }
    
    virtual void RenderFrame(){
        glm::vec3 x_result, p_result;
        computer.Compute(x_result, p_result);
        OpenGLShouldHaveNoError("DoCompute");
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        computer.Visualize(GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        char buf[100];
        static int counter = -1;
        
        sprintf(buf, "Step: %.4d", ++counter);
        glm::vec3 color(1.0f, 0.7f, 0.9f);
        GLTextRenderManager::AddText(NULL, buf, 50, -1 + sx * 50, 1 - 100 * sy, sx, sy,
                                     color);
        
        sprintf(buf, "X[0]: (%.4f, %.4f, %.4f)", x_result.x, x_result.y, x_result.z);
        GLTextRenderManager::AddText(NULL, buf, 50, -1 + sx * 50, 1 - 200 * sy, sx, sy,
                                     color);
        
        sprintf(buf, "P[0]: (%.4f, %.4f, %.4f)", p_result.x, p_result.y, p_result.z);
        GLTextRenderManager::AddText(NULL, buf, 50, -1 + sx * 50, 1 - 300 * sy, sx, sy,
                                     color);
        
        
        GLTextRenderManager::RenderText();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
