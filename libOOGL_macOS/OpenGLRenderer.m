//
//  OpenGLRenderer.m
//  libOOGL
//
//  Created by Haoyan Huo on 07/01/2017.
//  Copyright © 2017 Haoyan Huo. All rights reserved.
//

#import "OpenGLRenderer.h"

@interface OpenGLRenderer ()
{
    
    GLuint _viewWidth;
    GLuint _viewHeight;
    
}
@end

@implementation OpenGLRenderer


- (void) resizeWithWidth:(GLuint)width AndHeight:(GLuint)height
{
    glViewport(0, 0, width, height);
    
    _viewWidth = width;
    _viewHeight = height;
}

- (void) render
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

- (id) initWithDefaultFBO: (GLuint) defaultFBOName
{
    if((self = [super init]))
    {
        NSLog(@"%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));
        
        _defaultFBOName = defaultFBOName;
        
        _viewWidth = 100;
        _viewHeight = 100;
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClearColor(0.5f, 0.4f, 0.5f, 1.0f);
        
        [self render];
        
        
        // Check for errors to make sure all of our setup went ok
        GetGLError();
    }
    
    return self;
}


- (void) dealloc
{
    
}

@end
