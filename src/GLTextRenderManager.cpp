//
//  GLTextRenderManager.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/25/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLTextRenderManager.h"
#include "ft2build.h"
#include "GLShader.h"
#include FT_FREETYPE_H
#include <vector>
#include <unordered_map>

const char gTextShaderSource[] = R"THESHADER(
//!Shader!
//!VertexShader!
#version 400

layout(location=0) in vec4 coord;
out vec2 texcoord;

void main(void) {
    gl_Position = vec4(coord.xy, 0, 1);
    texcoord = coord.zw;
}
//!VertexShaderEnd!
//!FragmentShader!
#version 400

in vec2 texcoord;
out vec4 out_Color;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
    out_Color = vec4(1, 1, 1, texture(tex, texcoord).r) * color;
}
//!FragmentShaderEnd!
)THESHADER";


struct TextLine{
    bool widechar;
    std::wstring wstr;
    std::string str;
    int size;
    float x,y;
    float sx, sy;
    float color[4];
};

bool gIsRendererReady = false;
FT_Library gFtLibrary;
FT_Face gTypeface;
GLShader *gTextShader;
GLuint gTextureObject, gVertexBufferObject, gVertexArrayObject;
std::unordered_map<std::string, TextLine> gTexts;
std::vector<TextLine> gTextRenderOnce;

void GLTextRenderManager::SetFontname(const char *fontname){
    if(gIsRendererReady)
        return;
    
    GLTextRenderManager logger;
    
    if(FT_Init_FreeType(&gFtLibrary)) {
        logger.Error(__FUNCTION__, "Could not init freetype library");
    }
    
    if(FT_New_Face(gFtLibrary, fontname, 0, &gTypeface)) {
        logger.Error(__FUNCTION__, "Could not open font %s", fontname);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &gTextureObject);
    glBindTexture(GL_TEXTURE_2D, gTextureObject);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    gTextShader = new GLShader(gTextShaderSource, "tex\ncolor");
    
    gIsRendererReady = true;
}

void GLTextRenderManager::AddText(const char* name, const wchar_t* content, int size,
                                  float x, float y,
                                  float scale_x, float scale_y, glm::vec3 color){
    TextLine line;
    
    line.widechar = true;
    line.wstr = content;
    line.size = size;
    line.x = x;
    line.y = y;
    line.sx = scale_x;
    line.sy = scale_y;
    line.color[0] = color.x;
    line.color[1] = color.y;
    line.color[2] = color.z;
    line.color[3] = 1.0f;
    
    if(name){
        std::string key(name);
        gTexts[key] = line;
    }else
        gTextRenderOnce.push_back(line);
}

void GLTextRenderManager::AddText(const char *name, const char *content, int size,
                                  float x, float y, float scale_x, float scale_y,
                                  glm::vec3 color){
    TextLine line;
    line.widechar = false;
    line.str = content;
    line.size = size;
    line.x = x;
    line.y = y;
    line.sx = scale_x;
    line.sy = scale_y;
    line.color[0] = color.x;
    line.color[1] = color.y;
    line.color[2] = color.z;
    line.color[3] = 1.0f;
    
    if(name){
        std::string key(name);
        gTexts[key] = line;
    }else
        gTextRenderOnce.push_back(line);
}

void GLTextRenderManager::ClearText(const char *name){
    std::string key(name);
    auto item = gTexts.find(key);
    
    if(item == gTexts.end()){
        GLTextRenderManager logger;
        logger.Warning(__FUNCTION__, "Can't delete text with name: %s", name);
        return;
    }
    gTexts.erase(item);
}

void GLTextRenderManager::ClearText(){
    gTexts.clear();
    gTextRenderOnce.clear();
}

template<typename CharType, typename Container>
void RenderString(TextLine& line, Container& string){
    const CharType* s = string.c_str();
    FT_GlyphSlot g = gTypeface->glyph;
    float x = line.x, y = line.y;
    float sx = line.sx, sy = line.sy;
    
    FT_Set_Pixel_Sizes(gTypeface, 0, line.size);
    
    for(const CharType* p = s; *p; p++) {
        if(FT_Load_Char(gTypeface, *p, FT_LOAD_RENDER))
            continue;
        
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     g->bitmap.width,
                     g->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     g->bitmap.buffer
                     );
        
        float x2 = x + g->bitmap_left * sx;
        float y2 = - y - g->bitmap_top * sy;
        float w = g->bitmap.width * sx;
        float h = g->bitmap.rows * sy;
        
        GLfloat box[4][4] = {
            {x2,     -y2    , 0, 0},
            {x2 + w, -y2    , 1, 0},
            {x2,     -y2 - h, 0, 1},
            {x2 + w, -y2 - h, 1, 1},
        };
        
        gTextShader->Uniform4fv("color", 1, line.color);
        
        glBufferData(GL_ARRAY_BUFFER, 64, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
}

void GLTextRenderManager::RenderText(){
    GLTextRenderManager logger;
    
    gTextShader->UseProgram();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureObject);
    gTextShader->Uniform1i("tex", 0);
    
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    int oldAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    bool oldBlend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(auto line:gTextRenderOnce){
        if(line.widechar)
            RenderString<wchar_t>(line, line.wstr);
        else
            RenderString<char>(line, line.str);
        
        logger.OpenGLShouldHaveNoError(__FUNCTION__);
    }
    gTextRenderOnce.clear();
    
    for(auto line:gTexts){
        if(line.second.widechar)
            RenderString<wchar_t>(line.second, line.second.wstr);
        else
            RenderString<char>(line.second, line.second.str);
        
        logger.OpenGLShouldHaveNoError(__FUNCTION__);
    }
    
    glPixelStoref(GL_UNPACK_ALIGNMENT, oldAlignment);
    if (!oldBlend) {
        glDisable(GL_BLEND);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    logger.OpenGLShouldHaveNoError(__FUNCTION__);
}