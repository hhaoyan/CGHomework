//
//  MacOSX.mm
//  libOOGL
//
//  Created by Haoyan Huo on 3/24/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/NSBitmapImageRep.h>
#import "ObjectiveCInterface.h"

int DecodeImageFile(const char *filename, void **data, long* w, long* h, long* bpp)
{
    NSString* fn = [[NSString alloc]initWithUTF8String:filename];
    if(!fn)
        return 0;
    NSData *raw = [NSData dataWithContentsOfFile:fn];
    if(!raw)
        return 0;
    NSBitmapImageRep *rep = [NSBitmapImageRep imageRepWithData: raw];
    if(!rep)
        return 0;
    
    long bufferSize = [rep bytesPerPlane];
    void* buffer = malloc(bufferSize);
    memcpy(buffer, [rep bitmapData], [rep bytesPerPlane]);
    *data = buffer;
    *w = [rep pixelsWide];
    *h = [rep pixelsHigh];
    *bpp = [rep bitsPerPixel];
    return 1;
}

void FreeDecodedImage(void** data){
    free(*data);
    *data = NULL;
}