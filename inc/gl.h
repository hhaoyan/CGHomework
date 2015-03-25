//
//  gl.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/16/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

//  This file defines header files required to setup OpenGL
//  On OSX, windows platforms, source files needed to access
//  gl* functions should include this file, not glut/OpenGL
//  itself.

#ifndef __libOOGL__setup__
#define __libOOGL__setup__

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB // there is no arb support on MSVC
#endif

#ifdef WIN32
#include "GL/glew.h"
#endif

#include "GLFW/glfw3.h"

#ifdef WIN32
#define GLM_FORCE_PURE // due to a compiler complain of VS2013
#endif

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <stdio.h>

#endif
