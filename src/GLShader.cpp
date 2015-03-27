//
//  Shader.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLShader.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

const char SimpleColoredShaderSrc[] = R"THESHADER(
//!Shader!
//!VertexShader!
#version 400
layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;
layout(location=2) in vec3 in_Normal;

uniform mat4 transformMatrix;

void main(void)
{
    gl_Position = transformMatrix * vec4(in_Position, 1.0f);
}

//!VertexShaderEnd!

//!FragmentShader!
#version 400
uniform vec4 staticColor;

out vec4 out_Color;

void main(void){
    out_Color = staticColor;
}
//!FragmentShaderEnd!
)THESHADER";

const char SimpleTexturedShaderSrc[] = R"THESHADER(
//!Shader!
//!VertexShader!
#version 400
layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;
layout(location=2) in vec3 in_Normal;

uniform mat4 transformMatrix;
out vec2 texcoord;

void main(void)
{
    gl_Position = transformMatrix * vec4(in_Position, 1.0f);
    texcoord = in_Texcoord;
}

//!VertexShaderEnd!

//!FragmentShader!
#version 400
uniform sampler2D tex;

in vec2 texcoord;
out vec4 out_Color;

void main(void){
    out_Color = texture(tex, texcoord);
}
//!FragmentShaderEnd!
)THESHADER";

const char ShaderSign[] = "!Shader!";
const char VertexShaderStart[] = "!VertexShader!";
const char VertexShaderEnd[] = "!VertexShaderEnd!";
const char FragShaderStart[] = "!FragmentShader!";
const char FragShaderEnd[] = "!FragmentShaderEnd!";
const char GeomShaderStart[] = "!GeometryShader!";
const char GeomShaderEnd[] = "!GeometryShaderEnd!";
const char ComputeShaderStart[] = "!ComputeShader!";
const char ComputeShaderEnd[] = "!ComputeShaderEnd!";

GLShader* GLShader::SimpleShaderFactory::SimpleColoredShader(){
    GLShader* shader = new GLShader();
    shader->Compile(SimpleColoredShaderSrc, "transformMatrix\nstaticColor");
    return shader;
}

GLShader* GLShader::SimpleShaderFactory::SimpleTexturedShader(){
    GLShader* shader = new GLShader(SimpleTexturedShaderSrc, "transformMatrix\ntex");
    return shader;
}

GLShader::GLShader() : fProgram(-1), fIsAttached(false){
    memset(fShaders, -1, sizeof(fShaders));
}

GLShader::GLShader(const char* source, const char* uniforms):fIsAttached(false){
    memset(fShaders, -1, sizeof(fShaders));
    fProgram = -1;
    
    if(!Compile(source, uniforms))
        Error(__FUNCTION__, "Failed to compile shader %s", source);
}

void GLShader::DeleteObject(){
    if(IsAttached())
        Detach();
}

void GLShader::Detach(){
    if(fProgram != -1)
        glDeleteProgram(fProgram);
    
    for(auto shader:fShaders){
        if(shader != -1)
            glDeleteShader(shader);
    }
    fIsAttached = false;
}

bool IsFile(std::string& str){
    return str.find(ShaderSign) == std::string::npos;
}

void GLShader::UseProgram(){
    glUseProgram(fProgram);
    OpenGLShouldHaveNoError(__FUNCTION__);
}

GLint GLShader::GetUniform(std::string name){
    // TODO: are we suffering performance issue?
    auto item = fUniforms.find(name);
    if(item == fUniforms.end())
        Error(__FUNCTION__, "can't find uniform name %s", name.c_str());
    
    return item->second;
}

bool GLShader::Compile(const char *src, const char *uniforms){
    std::string source(src);
    if (IsFile(source)) {
        std::ifstream fin(src);
        if(fin.fail()){
            Warning(__FUNCTION__, "Shader file %s doesn't exist!", src);
            return false;
        }
        
        fin.seekg(0, std::ios::end);
        size_t filesize = fin.tellg();
        if(filesize >= 1024 * 1024){
            fin.close();
            Warning(__FUNCTION__, "Shader file %s is too big, not loaded", src);
            return false;
        }
        
        source.clear(); source.reserve(filesize);
        fin.seekg(0);
        source.assign(std::istreambuf_iterator<char>(fin),
                      std::istreambuf_iterator<char>());
        fin.close();
    }
    
    // now we have source code
    
    {
        size_t vertexShaderStart, vertexShaderEnd;
        
        vertexShaderStart = source.find(VertexShaderStart);
        if(vertexShaderStart != std::string::npos){
            vertexShaderStart += strlen(VertexShaderStart);
            
            vertexShaderEnd = source.find(VertexShaderEnd);
            if (vertexShaderEnd == std::string::npos) {
                Error(__FUNCTION__, "Invalid shader file %s: vs has no end", src);
            }
            vertexShaderEnd += strlen(VertexShaderEnd);
            
            std::string vs(source.begin() + vertexShaderStart,
                           source.begin() + vertexShaderEnd);
            
            fShaders[kVertexShader] = glCreateShader(GL_VERTEX_SHADER);
            const char* vs_source = vs.c_str();
            glShaderSource(fShaders[kVertexShader], 1, (const char**)&vs_source, 0);
            glCompileShader(fShaders[kVertexShader]);
            
            GLint status;
            glGetShaderiv(fShaders[kVertexShader], GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char e[500];
                glGetShaderInfoLog(fShaders[kVertexShader], 500, 0, e);
                // clear the error
                glGetError();
                Warning(__FUNCTION__, "compile vs %s failed: %s(Truncated at 500 chars)", src, e);
                return false;
            }
        }
    }
    
    {
        size_t fragShaderStart, fragShaderEnd;
        
        fragShaderStart = source.find(FragShaderStart);
        if(fragShaderStart != std::string::npos){
            fragShaderStart += strlen(FragShaderStart);
            
            fragShaderEnd = source.find(FragShaderEnd);
            if (fragShaderEnd == std::string::npos) {
                Error(__FUNCTION__, "Invalid shader file %s: fs has no end", src);
            }
            
            std::string fs(source.begin() + fragShaderStart,
                           source.begin() + fragShaderEnd);
            
            fShaders[kFragmentShader] = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_source = fs.c_str();
            glShaderSource(fShaders[kFragmentShader], 1, (const char**)&fs_source, 0);
            glCompileShader(fShaders[kFragmentShader]);
            
            GLint status;
            glGetShaderiv(fShaders[kFragmentShader], GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char e[500];
                glGetShaderInfoLog(fShaders[kFragmentShader], 500, 0, e);
                // clear the error
                glGetError();
                Warning(__FUNCTION__, "compile fs %s failed: %s(Truncated at 500 chars)", src, e);
                return false;
            }
        }
    }
    
    {
        size_t geomShaderStart, geomShaderEnd;
        
        geomShaderStart = source.find(GeomShaderStart);
        if(geomShaderStart != std::string::npos){
            geomShaderStart += strlen(GeomShaderStart);
            
            geomShaderEnd = source.find(GeomShaderEnd);
            if (geomShaderEnd == std::string::npos) {
                Error(__FUNCTION__, "Invalid shader file %s: fs has no end", src);
            }
            
            std::string gs(source.begin() + geomShaderStart,
                           source.begin() + geomShaderEnd);
            
            fShaders[kGeometryShader] = glCreateShader(GL_GEOMETRY_SHADER);
            const char* gs_source = gs.c_str();
            glShaderSource(fShaders[kGeometryShader], 1, (const char**)&gs_source, 0);
            glCompileShader(fShaders[kGeometryShader]);
            
            GLint status;
            glGetShaderiv(fShaders[kGeometryShader], GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char e[500];
                glGetShaderInfoLog(fShaders[kGeometryShader], 500, 0, e);
                // clear the error
                glGetError();
                Warning(__FUNCTION__, "compile gs %s failed: %s(Truncated at 500 chars)", src, e);
                return false;
            }
        }
    }
    
    // no compute shader support on OS X
    if(fShaders[kVertexShader] == -1 && fShaders[kFragmentShader] == -1 &&
       fShaders[kGeometryShader] == -1){
        Warning(__FUNCTION__, "No shader compiled for %s", src);
        return false;
    }
    
    {
        fProgram = glCreateProgram();
        if(fShaders[kVertexShader] != -1)
            glAttachShader(fProgram, fShaders[kVertexShader]);
        if(fShaders[kFragmentShader] != -1)
            glAttachShader(fProgram, fShaders[kFragmentShader]);
        if(fShaders[kGeometryShader] != -1)
            glAttachShader(fProgram, fShaders[kGeometryShader]);
    
        glLinkProgram(fProgram);
    
        GLint status;
        glGetProgramiv(fProgram, GL_LINK_STATUS, &status);
        
        if(status !=  GL_TRUE){
            char e[500];
            glGetProgramInfoLog(fProgram, 500, 0, e);
            // clear the error
            glGetError();
            Warning(__FUNCTION__, "link shader %s failed: %s(Truncated at 500 chars)", src, e);
            return false;
        }
    }
    
    std::string uniformStr(uniforms);
    std::istringstream iss(uniformStr);
    std::vector<std::string> uniformNames{std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}};
    
    for(auto uniform:uniformNames){
        GLint uid = glGetUniformLocation(fProgram, uniform.c_str());
        if(uid < 0){
            GLenum error = glGetError();
            Error(__FUNCTION__, "(%d) can't get uniform %s for shader %s", error, uniform.c_str(), src);
        }
        
        fUniforms[uniform] = uid;
    }
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    return true;
}

void GLShader::Uniform1f(const char *name, GLfloat f0){
    glUniform1f(GetUniform(name), f0);
}

void GLShader::Uniform2f(const char *name, GLfloat f0, GLfloat f1){
    glUniform2f(GetUniform(name), f0, f1);
}

void GLShader::Uniform3f(const char *name, GLfloat f0, GLfloat f1, GLfloat f2){
    glUniform3f(GetUniform(name), f0, f1, f2);
}

void GLShader::Uniform4f(const char *name, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3){
    glUniform4f(GetUniform(name), f0, f1, f2, f3);
}

void GLShader::Uniform1i(const char *name, GLint i0){
    glUniform1i(GetUniform(name), i0);
}

void GLShader::Uniform2i(const char *name, GLint i0, GLint i1){
    glUniform2i(GetUniform(name), i0, i1);
}

void GLShader::Uniform3i(const char *name, GLint i0, GLint i1, GLint i2){
    glUniform3i(GetUniform(name), i0, i1, i2);
}

void GLShader::Uniform4i(const char *name, GLint i0, GLint i1, GLint i2, GLint i3){
    glUniform4i(GetUniform(name), i0, i1, i2, i3);
}

void GLShader::Uniform1fv(const char *name, GLsizei count, const GLfloat *f){
    glUniform1fv(GetUniform(name), count, f);
}

void GLShader::Uniform2fv(const char *name, GLsizei count, const GLfloat *f){
    glUniform2fv(GetUniform(name), count, f);
}

void GLShader::Uniform3fv(const char *name, GLsizei count, const GLfloat *f){
    glUniform3fv(GetUniform(name), count, f);
}

void GLShader::Uniform4fv(const char *name, GLsizei count, const GLfloat *f){
    glUniform4fv(GetUniform(name), count, f);
}

void GLShader::Uniform1iv(const char *name, GLsizei count, const GLint *i){
    glUniform1iv(GetUniform(name), count, i);
}

void GLShader::Uniform2iv(const char *name, GLsizei count, const GLint *i){
    glUniform2iv(GetUniform(name), count, i);
}

void GLShader::Uniform3iv(const char *name, GLsizei count, const GLint *i){
    glUniform3iv(GetUniform(name), count, i);
}

void GLShader::Uniform4iv(const char *name, GLsizei count, const GLint *i){
    glUniform4iv(GetUniform(name), count, i);
}

void GLShader::UniformMatrix2fv(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
    glUniformMatrix2fv(GetUniform(name), count, transpose, value);
}

void GLShader::UniformMatrix3fv(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
    glUniformMatrix3fv(GetUniform(name), count, transpose, value);
}

void GLShader::UniformMatrix4fv(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value){
    glUniformMatrix4fv(GetUniform(name), count, transpose, value);
}