//
//  RenderViewController.h
//  libOOGL
//
//  Created by Haoyan Huo on 07/01/2017.
//  Copyright © 2017 Haoyan Huo. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface RenderViewController : NSOpenGLView {
    CVDisplayLinkRef displayLink;
}

@end
