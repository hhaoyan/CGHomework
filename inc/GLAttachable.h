//
//  GLAttachable.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/27/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_GLAttachable_h
#define libOOGL_GLAttachable_h

/**
 * @brief Interface of a OpenGL attachable resource.
 *
 * A OpenGL resource in "Detached" mode means it's not in
 * OpenGL registry, however OpenGL objects could be created
 * by calling Attach().
 *
 * Before the OpenGL context is created, any call to gl* functions
 * is forbidden, but resources like meshes, textures could have buffer
 * prepared in memory, to reduce OpenGL object init time.
 */
class GLAttachable{
public:
    virtual void Attach() = 0;
    virtual void Detach() = 0;
    virtual bool IsAttached() = 0;
};

#endif
