//
//  GLApplication.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLApplication.h"

GLApplication* GLApplication::fInstance = 0;

#ifdef __llvm__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __llvm__

GLApplication::GLApplication() :
fWindow(0){
    // set up OpenGL environment and do necessary stuffs.
    
	// TODO: we can't do this in initializer because a compiler bug of VS2013, 
	// maybe this will be fixed in next generation of VS
	fWindowSize[0] = 0; fWindowSize[1] = 0;
	fMousePosition[0] = fMousePosition[1] = 0;
	fWindowFramebufferSize[0] = fWindowFramebufferSize[1] = 0;

    memset(fKeyState, 0, sizeof(fKeyState));
    memset(fMouseState, 0, sizeof(fMouseState));
    
    if(fInstance)
        Error(__FUNCTION__, "More than one GL app is instantiated");
    
    fInstance = this;
    glfwSetErrorCallback(GLApplication::ErrorCallback);
    if (!glfwInit())
        Error(__FUNCTION__, "Can't initialize GLFW");
}

GLApplication::~GLApplication(){
    glfwTerminate();
}

void GLApplication::CreateWindow(const char* title, int width, int height){
    if(width <= 0)
        Error(__FUNCTION__, "Width should not be %d", width);
    if(height <= 0)
        Error(__FUNCTION__, "Height should not be %d", height);
    
    fWindowSize[0] = width;
    fWindowSize[1] = height;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    fWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    
    glfwGetFramebufferSize(fWindow, fWindowFramebufferSize, fWindowFramebufferSize+1);
    
    if(!fWindow)
        Error(__FUNCTION__, "Could not create window");
    
    glfwMakeContextCurrent(fWindow);
    glfwSwapInterval(1);
    
    glfwSetMouseButtonCallback(fWindow, GLApplication::MouseCallback);
    glfwSetWindowSizeCallback(fWindow, GLApplication::ResizeCallback);
    glfwSetKeyCallback(fWindow, GLApplication::KeyboardCallback);
    glfwSetScrollCallback(fWindow, GLApplication::ScrollCallback);
    glfwSetCursorPosCallback(fWindow, GLApplication::MotionCallback);
    glfwSetFramebufferSizeCallback(fWindow, GLApplication::FramebufferSizeCallback);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    
    Info(__FUNCTION__, "OpenGL version: %s", glGetString(GL_VERSION));
}

void GLApplication::RunMainLoop(){
    if(!fWindow)
        Error(__FUNCTION__, "Window not created");
    
    CreateApplication();
    /**
     * OpenGL does not have depth test switched on by default, so if you want to 
     * enable depth testing, call glEnable(GL_DEPTH_TEST) at CreateApplication().
     *
     */
    //glEnable(GL_DEPTH_TEST);
    OpenGLShouldHaveNoError("InitializeApplication");
    
    while ((!glfwWindowShouldClose(fWindow))) {
        RenderFrame();
        
        glfwSwapBuffers(fWindow);
        glfwPollEvents();
        
        OpenGLShouldHaveNoError(__FUNCTION__);
    }
    
    ShutdownApplication();
    glfwDestroyWindow(fWindow);
    OpenGLShouldHaveNoError("ShutdownApplication");
}

void GLApplication::Resize(GLFWwindow *window, int width, int height){
    fWindowSize[0] = width;
    fWindowSize[1] = height;
    
    WindowSizeChanged(width, height);
}

void GLApplication::Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods){
    fKeyState[key] = GLFW_PRESS;
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:
            KeyDown(key);
            break;
            
        case GLFW_RELEASE:
            KeyUp(key);
            break;
            
        default:
            break;
    }
}

void GLApplication::Motion(GLFWwindow *window, double x, double y){
    fMousePosition[0] = x;
    fMousePosition[1] = y;
    MouseMove(x, y);
}

void GLApplication::Mouse(GLFWwindow *window, int button, int action, int mods){
    int which;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            which = 0;
            break;
            
        case GLFW_MOUSE_BUTTON_MIDDLE:
            which = 1;
            break;
            
        case GLFW_MOUSE_BUTTON_RIGHT:
            which = 2;
            break;
            
        default:
            Warning(__FUNCTION__, "Unknown mouse button %d from GLFW", button);
            return;
    }
    
    switch (action) {
        case GLFW_PRESS:
            fMouseState[which] = 1;
            MouseKeyDown(which);
            break;
            
        case GLFW_RELEASE:
            fMouseState[which] = 0;
            MouseKeyUp(which);
            break;
            
        default:
            Warning(__FUNCTION__, "Unknown moust button state %d from GLFW", action);
            return;
    }
}

void GLApplication::Scroll(GLFWwindow *window, double xoffset, double yoffset){
    MouseScroll(xoffset, yoffset);
}

void GLApplication::FramebufferSize(GLFWwindow *window, int width, int height){
    fWindowFramebufferSize[0] = width;
    fWindowFramebufferSize[1] = height;
    WindowFramebufferSizeChanged(width, height);
}

#ifdef __llvm__
#pragma clang diagnostic pop
#endif // __llvm__
