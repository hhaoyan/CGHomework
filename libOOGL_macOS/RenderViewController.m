//
//  RenderViewController.m
//  libOOGL
//
//  Created by Haoyan Huo on 07/01/2017.
//  Copyright © 2017 Haoyan Huo. All rights reserved.
//

#import "RenderViewController.h"
#import "OpenGLRenderer.h"

#define SUPPORT_RETINA_RESOLUTION 1

@interface RenderViewController ()
{
    OpenGLRenderer* _renderer;
}
@end

@implementation RenderViewController


- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
    @autoreleasepool {
        [self drawView];
    }
    return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
    CVReturn result = [(__bridge RenderViewController*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) awakeFromNib
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
#endif
        0
    };
    
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    if (!pf)
    {
        NSLog(@"No OpenGL pixel format");
    }
	   
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 && defined(DEBUG)
    CGLEnable([context CGLContextObj], kCGLCECrashOnRemovedFunctions);
#endif
    
    [self setPixelFormat:pf];
    
    [self setOpenGLContext:context];
    
#if SUPPORT_RETINA_RESOLUTION
    [self setWantsBestResolutionOpenGLSurface:YES];
#endif // SUPPORT_RETINA_RESOLUTION
}

- (void) prepareOpenGL
{
    [super prepareOpenGL];
    
    [self initGL];
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    CVDisplayLinkStart(displayLink);
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:[self window]];
}

- (void) windowWillClose:(NSNotification*)notification
{
    CVDisplayLinkStop(displayLink);
}

- (void) initGL
{
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    _renderer = [[OpenGLRenderer alloc] initWithDefaultFBO:0];
}

- (void)reshape
{
    [super reshape];
    
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    NSRect viewRectPoints = [self bounds];
    
#if SUPPORT_RETINA_RESOLUTION
    
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
#else //if !SUPPORT_RETINA_RESOLUTION
    
    NSRect viewRectPixels = viewRectPoints;
    
#endif // !SUPPORT_RETINA_RESOLUTION
    
    [_renderer resizeWithWidth:viewRectPixels.size.width
                     AndHeight:viewRectPixels.size.height];
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}


- (void)renewGState
{
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}

- (void) drawRect: (NSRect) theRect
{
    [self drawView];
}

- (void) drawView
{
    [[self openGLContext] makeCurrentContext];
    
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    [_renderer render];
    
    CGLFlushDrawable([[self openGLContext] CGLContextObj]);
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (void) dealloc
{
    CVDisplayLinkStop(displayLink);
    
    CVDisplayLinkRelease(displayLink);
}
@end
