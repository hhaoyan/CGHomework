//
//  gl.h
//  OpenGLFirst
//
//  Created by Haoyan Huo on 3/16/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

//  This file defines header files required to setup OpenGL
//  On OSX, windows platforms, source files needed to access
//  gl* functions should include this file, not glut/OpenGL
//  itself.

#ifndef HEADER_GLSETUP
#define HEADER_GLSETUP

#ifdef __APPLE__

#include <Opengl/gl3.h>
#define __gl_h_
#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#else
#error "OpenGL not implemented on your platform"

#endif //APPLE

#endif
