//
//  OpenGLRenderer.h
//  libOOGL
//
//  Created by Haoyan Huo on 07/01/2017.
//  Copyright © 2017 Haoyan Huo. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "BaseGLImports.h"

@interface OpenGLRenderer : NSObject

@property (nonatomic) GLuint defaultFBOName;

- (instancetype) initWithDefaultFBO: (GLuint) defaultFBOName;
- (void) resizeWithWidth:(GLuint)width AndHeight:(GLuint)height;
- (void) render;
- (void) dealloc;

@end
