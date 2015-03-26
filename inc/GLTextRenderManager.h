//
//  GLTextRenderManager.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/25/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_GLTextRenderManager_h
#define libOOGL_GLTextRenderManager_h

#include "GLObject.h"
#include "gl.h"

/**
 * @brief Manages text rendering.
 *
 * A pure static class. Before any text could be rendered, you must setup
 * the typeface(AKA font name) by calling SetFontname().
 *
 * Coordinates of texts are computed using screen relative units. In horizontal
 * direction, x starts with -1 at left and ends with +1 at right, in vertical
 * direction, y starts with -1 at bottom and ends with +1 at top.
 */
class GLTextRenderManager : GLObject{
public:
    /**
     * @brief Sets the font name of text renderer.
     *
     * This clears all texts and makes the freetype library to be initialized,
     * should be called when application starts, further calls makes no effects.
     * @param[in] fontname The font filename.
     */
    static void SetFontname(const char* fontname);
    
    /**
     * @brief Adds a line to text renderer.
     *
     * This adds a line to the screen. if name is NULL, then
     * this line will only be draw once, otherwise the text is persistent.
     * @param[in] name the name of text, could be NULL.
     * @param[in] content the content of text.
     * @param[in] size The text size.
     * @param[in] x X of start point, in screen relative coordinate.
     * @param[in] y Y of start point, in screen relative coordinate.
     * @param[in] scale_x Scale factor of X direction.
     * @param[in] scale_y Scale factor of Y direction.
     * @param[in] color The text color;
     */
    static void AddText(const char* name, const wchar_t* content, int size,
                        float x, float y,
                        float scale_x, float scale_y,
                        glm::vec3 color=glm::vec3(0.0f, 0.0f, 0.0f));
    
    /**
     * @brief Adds a line to text renderer.
     *
     * This adds a line to the screen. if name is NULL, then
     * this line will only be draw once, otherwise the text is persistent.
     * @param[in] name the name of text, could be NULL.
     * @param[in] content the content of text.
     * @param[in] size The text size.
     * @param[in] x X of start point, in screen relative coordinate.
     * @param[in] y Y of start point, in screen relative coordinate.
     * @param[in] scale_x Scale factor of X direction.
     * @param[in] scale_y Scale factor of Y direction.
     * @param[in] color The text color;
     */
    static void AddText(const char* name, const char* content, int size,
                        float x, float y,
                        float scale_x, float scale_y,
                        glm::vec3 color=glm::vec3(0.0f, 0.0f, 0.0f));
    
    /**
     * @brief Clears a line of text.
     * @param[in] name The name of text, should not be NULL.
     * @see AddText()
     */
    static void ClearText(const char* name);
    
    /**
     * @brief Clears all texts.
     */
    static void ClearText();
    
    /**
     * @brief Render all text.
     *
     * Call this in the render pass of your application.
     */
    static void RenderText();
};

#endif
