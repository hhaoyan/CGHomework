//
//  GLObject.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __OpenGLFirst__GLObject__
#define __OpenGLFirst__GLObject__

#include <stdio.h>
#include "gl.h"

class GLApplication;

/*!
 * @brief The root class for all GL objects.
 *
 * GLObject is the root class of all OpenGL objects. An OpenGL object
 * are something in OpenGL context that could be passed to gl*
 * functions. 
 *
 * GLObject has two states: attached and unattached: attached objects are alive
 * in OpenGL context, which means it can directly be used to gl* draw calls;
 * unattached objects, on the other hand, are not, they may have some
 * buffers in memory but it must be explicitly "attached" to OpenGL context.
 *
 * Derived classes are recommended to implement DeleteObject() virtual
 * method, which is automatically called when the OpenGL context is going
 * to be deleted, at this time all OpenGL objects are required to be gracefully
 * freed. All derived classes are required to not make any gl* calles during 
 * object destructing, because GL context may be not present.
 */
class GLObject{
public:
    /*! Default destructor */
    ~GLObject(){
        DeleteObject();
    }
    
protected:
    /*! 
     * @brief Log messages on console.
     * @param what the log levelname.
     * @param who the logger name.
     * @param how the formatted log message string.
     * @see Info()
     * @see Warning()
     * @see Error()
     */
    void Log(const char* what, const char* who, const char* how, ...);
    
    /*! 
     * @brief Log info messages on console.
     * @param who the logger name.
     * @param how the formatted log message string.
     * @see Log()
     */
    void Info(const char* who, const char* how, ...);
    
    /*! 
     * @brief Log warning messages on console.
     * @param who the logger name.
     * @param how the formatted log message string.
     * @see Log()
     */
    void Warning(const char* who, const char* how, ...);
    
    /*! 
     * @brief Log error messages on console, also shutting down the application.
     * @param who the logger name.
     * @param how the formatted log message string.
     * @see Log()
     */
    void Error(const char* who, const char* how, ...);
    
    /*! 
     * @brief Make sure no OpenGL error happened.
     * 
     * Sometimes it's necessary to make sure no OpenGL error just happened, 
     * OpenGL errors will be treated as fatal error, same as a call to Error().
     * @param who the logger name.
     * @see Error()
     */
    void OpenGLShouldHaveNoError(const char* who);
    
    /*! 
     * @brief Override this function to do GL object destructing.
     * 
     * GLObject classes should not do any gl* calls in destructors, because
     * the OpenGL context may not be present, instead they should free OpenGL
     * objects in this virtual function, which will be called by framework at
     * a proper time.
     */
    virtual void DeleteObject() {}
};

#endif /* defined(__OpenGLFirst__GLObject__) */
