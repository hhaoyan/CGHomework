//
//  GLApplication.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __OpenGLFirst__GLApplication__
#define __OpenGLFirst__GLApplication__

#include <stdio.h>
#include "gl.h"
#include "GLObject.h"

/**
 * @brief GLApplication class is the base class of a complete OpenGL application. 
 *
 * GLApplication registers the OpenGL context and GLUT, installs callbacks, and 
 * dispachs every call of drawing of a frame. There should always be only one
 * GLApplication instance during the lifecycle of application.
 *
 * GLApplication also has a reference to the window created by GLUT. Also the input
 * events will be received by GLApplication and send to client codes.
 */
class GLApplication : public GLObject{
public:
    /** @brief default constructor */
    GLApplication(int argc = NULL, char** argv = NULL);
    /** @brief default destructor */
    virtual ~GLApplication();
    
    /** @brief this method creates the GLUT window, also initializes the OpenGL context. */
    void CreateWindow(const char* title, int width, int height);
    /** @brief enter the main GLUT event loop. */
    void RunMainLoop();
    
    /** @brief Gets the key state. */
    bool IsKeyDown(unsigned char chr) {return fKeyState[chr] == 1;}
    /** @brief Gets the mouse key state. */
    bool IsMouseKeyDown(int which)
    {return (which <= 2 && which >= 0) ? (fMouseState[which] == 1) : false;}
    
    /** @brief Gets the window width.
     * @see GetWindowHeight()
     */
    int GetWindowWidth() {return fWindowSize[0];}
    /** @brief Gets the window height.
     * @see GetWindowWidth()
     */
    int GetWindowHeight() {return fWindowSize[1];}
    /** @brief Gets the pointer X coordinate.
     * @see GetPointerY()
     */
    int GetPointerX() {return fMousePosition[0];}
    /** @brief Gets the pointer Y coordinate.
     * @see GetPointerX()
     */
    int GetPointerY() {return fMousePosition[1];}
    
protected:
    
    /** @brief Event callback when the application is created and OpenGL context
     * initialized.
     *
     * Initialize important objects and get ready for rendering during this callback.
     * Three fundamental callbacks that child classes must implement are CreateApplication(),
     * RenderFrame(), and ShutdownApplication(). And those virtual methods could be 
     * implemented for conveience:
     *  - WindowSizeChanged()
     *  - MouseKeyDown()
     *  - MouseKeyUp()
     *  - KeyDown()
     *  - KeyUp()
     *  - MouseMove()
     *  - MouseScrollUp()
     *  - MouseScrollDown()
     */
    virtual void CreateApplication() = 0;
    /** @brief Event callback of every frame draw event.
     *
     * Draw the framebuffer in this callback. But try not doing heavy works.
     */
    virtual void RenderFrame() = 0;
    /** @brief Event callback when the application is going to die.
     *
     * Free the objects created in CreateApplication() during this callback.
     */
    virtual void ShutdownApplication() = 0;
    
    /** @brief Event callback when the window size has changed.
     * @param[in] width the new window width.
     * @param[in] height the new window height.
     */
    virtual void WindowSizeChanged(int width, int height) {return;}
    
    /** @brief Event callback when a mouse key is pressed.
     * @param[in] which the key id, 0 stands for left button, and 1 the
     * middle button, 2 the right button.
     * @see MouseKeyUp()
     */
    virtual void MouseKeyDown(int which) {return;}
    
    /** @brief Event callback when a mouse key is released.
     * @param[in] which the key id.
     * @see MouseKeyDown()
     */
    virtual void MouseKeyUp(int which) {return;}
    
    /** @brief Event callback when a keyboard key is pressed.
     * @param[in] which the key char.
     * @see KeyUp()
     */
    virtual void KeyDown(unsigned char which) {return;}
    
    /** @brief Event callback when a keyboard key is release.
     * @param[in] which the key char.
     * @see KeyDown()
     */
    virtual void KeyUp(unsigned char which) {return;}
    
    /** @brief Event callback when the pointer moves in window.
     * @param[in] x the x coordinate of pointer.
     * @param[in] y the y coordinate of pointer.
     */
    virtual void MouseMove(int x, int y) {return;}
    
    /** @brief Event callback when mouse scrolls up. 
     *
     * Note: OSX does not have scroll events.
     * @see MouseScrollDown()
     */
    virtual void MouseScrollUp() {return;}
    
    /** @brief Event callback when mouse scrolls down.
     * @see MouseScrollUp()
     */
    virtual void MouseScrollDown() {return;}
    
private:
    /** @brief The singleton pointer of GLApplication. */
    static GLApplication* fInstance;
    
    /** @brief Gets the unique GLApplication pointer of this process. */
    static GLApplication* instance(){return fInstance;}
    
    void Reshape(int width, int height);
    void Display();
    void Keyboard(unsigned char chr, int x, int y);
    void KeyboardUp(unsigned char chr, int x, int y);
    void Mouse(int button, int state, int x, int y);
    void Motion(int x, int y);
    
    // static callbacks for GLUT
    static void ReshapeCallback(int width, int height) {instance()->Reshape(width, height);}
    static void DisplayCallback() {instance()->Display();}
    static void KeyboardCallback(unsigned char chr, int x, int y) {instance()->Keyboard(chr, x, y);}
    static void KeyboardUpCallback(unsigned char chr, int x, int y) {instance()->KeyboardUp(chr, x, y);}
    static void MouseCallback(int btn, int state, int x, int y) {instance()->Mouse(btn, state, x, y);}
    static void MotionCallback(int x, int y) {instance()->Motion(x, y);}
    
    int fWindowHandle;
    int fWindowSize[2];
    unsigned char fKeyState[256], fMouseState[3];
    int fMousePosition[2];
};

#endif /* defined(__OpenGLFirst__GLApplication__) */
