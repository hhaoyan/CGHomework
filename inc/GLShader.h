//
//  Shader.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __libOOGL__Shader__
#define __libOOGL__Shader__

#include "gl.h"
#include "GLObject.h"
#include <unordered_map>

class GLShader : public GLObject{
public:
    class SimpleShaderFactory{
    public:
        // this creates a simple colored shader with uniform
        // "transformMatrix" and "staticColor"
        static GLShader* SimpleColoredShader();
        
        // uniform: tex
        static GLShader* SimpleTexturedShader();
    };
    GLShader();
    GLShader(const char* source, const char* uniforms);
    
    bool Compile(const char* source, const char* uniforms);
    
    GLint GetUniform(std::string name);
    
    // gl* API
    void Uniform1i(const char* name, GLint i0);
    void Uniform2i(const char* name, GLint i0, GLint i1);
    void Uniform3i(const char* name, GLint i0, GLint i1, GLint i2);
    void Uniform4i(const char* name, GLint i0, GLint i1, GLint i2, GLint i3);
    void Uniform1f(const char* name, GLfloat f0);
    void Uniform2f(const char* name, GLfloat f0, GLfloat f1);
    void Uniform3f(const char* name, GLfloat f0, GLfloat f1, GLfloat f2);
    void Uniform4f(const char* name, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);
    void Uniform1fv(const char* name, GLsizei count, const GLfloat* f);
    void Uniform2fv(const char* name, GLsizei count, const GLfloat* f);
    void Uniform3fv(const char* name, GLsizei count, const GLfloat* f);
    void Uniform4fv(const char* name, GLsizei count, const GLfloat* f);
    void Uniform1iv(const char* name, GLsizei count, const GLint* i);
    void Uniform2iv(const char* name, GLsizei count, const GLint* i);
    void Uniform3iv(const char* name, GLsizei count, const GLint* i);
    void Uniform4iv(const char* name, GLsizei count, const GLint* i);
    void UniformMatrix2fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat *value);
    void UniformMatrix3fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat *value);
    void UniformMatrix4fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat *value);
    
    void UseProgram();
    
    void Detach();
    
protected:
    virtual void DeleteObject();
    
private:
    enum eShaderType{
        kVertexShader = 0,
        kFragmentShader,
        kGeometryShader,
        kComputeShader,
    };
    
    GLint       fShaders[sizeof(eShaderType)];
    GLint       fProgram;
    std::unordered_map<std::string, GLuint> fUniforms;
    typedef std::unordered_map<std::string, GLuint> fMapType;
};

#endif /* defined(__OpenGLFirst__Shader__) */
