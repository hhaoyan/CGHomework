//
//  GLApplication.cpp
//  OpenGLFirst
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

GLApplication::GLApplication(int argc, char** argv) :
fWindowHandle(0), fWindowSize{0,0}, fMousePosition{0,0}{
    // set up OpenGL environment and do necessary stuffs.
    
    glutInit(&argc, argv);
    memset(fKeyState, 0, sizeof(fKeyState));
    memset(fMouseState, 0, sizeof(fMouseState));
    
    if(fInstance)
        Error(__FUNCTION__, "More than one GL app is instantiated");
    
    fInstance = this;
}

GLApplication::~GLApplication(){
    
}

void GLApplication::CreateWindow(const char* title, int width, int height){
    // initialize OpenGL context and create the window
    
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    
    if(width <= 0)
        Error(__FUNCTION__, "Width should not be %d", width);
    if(height <= 0)
        Error(__FUNCTION__, "Height should not be %d", height);
    
    fWindowSize[0] = width;
    fWindowSize[1] = height;
    glutInitWindowSize(width, height);
    
    fWindowHandle = glutCreateWindow(title);
    
    if(fWindowHandle < 1)
        Error(__FUNCTION__, "Could not create window");
    
    glutReshapeFunc(GLApplication::ReshapeCallback);
    glutDisplayFunc(GLApplication::DisplayCallback);
    glutKeyboardFunc(GLApplication::KeyboardCallback);
    glutKeyboardUpFunc(GLApplication::KeyboardUpCallback);
    glutMotionFunc(GLApplication::MotionCallback);
    glutMouseFunc(GLApplication::MouseCallback);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    
    Info(__FUNCTION__, "OpenGL version: %s", glGetString(GL_VERSION));
}

void GLApplication::RunMainLoop(){
    if(fWindowHandle < 1)
        Error(__FUNCTION__, "Window not created");
    
    CreateApplication();
    // It seems that OpenGL have depth test switched off after the
    // context is created
    glEnable(GL_DEPTH_TEST);
    OpenGLShouldHaveNoError("InitializeApplication");
    
    glutMainLoop();
    
    ShutdownApplication();
    OpenGLShouldHaveNoError("ShutdownApplication");
}

void GLApplication::Display(){
    RenderFrame();
    glutSwapBuffers();
    glutPostRedisplay();
    OpenGLShouldHaveNoError(__FUNCTION__);
}

void GLApplication::Reshape(int width, int height){
    fWindowSize[0] = width;
    fWindowSize[1] = height;
    
    WindowSizeChanged(width, height);
}

void GLApplication::Keyboard(unsigned char chr, int x, int y){
    fKeyState[chr] = 1;
    KeyDown(chr);
}

void GLApplication::KeyboardUp(unsigned char chr, int x, int y){
    fKeyState[chr] = 0;
    KeyUp(chr);
}

void GLApplication::Motion(int x, int y){
    fMousePosition[0] = x;
    fMousePosition[1] = y;
    MouseMove(x, y);
}

void GLApplication::Mouse(int button, int state, int x, int y){
    // update mouse pointer location, but do not notify app
    fMousePosition[0] = x;
    fMousePosition[1] = y;
    
    // TODO: implement wheel scroll events
    int which;
    switch (button) {
        case GLUT_LEFT_BUTTON:
            which = 0;
            break;
            
        case GLUT_MIDDLE_BUTTON:
            which = 1;
            break;
            
        case GLUT_RIGHT_BUTTON:
            which = 2;
            break;
            
        default:
            Warning(__FUNCTION__, "Unknown mouse button %d from GLUT", button);
            return;
    }
    
    switch (state) {
        case GLUT_UP:
            fMouseState[which] = 0;
            MouseKeyUp(which);
            break;
            
        case GLUT_DOWN:
            fMouseState[which] = 1;
            MouseKeyDown(which);
            break;
            
        default:
            Warning(__FUNCTION__, "Unknown moust button state %d from GLUT", state);
            return;
    }
}

#ifdef __llvm__
#pragma clang diagnostic pop
#endif // __llvm__
