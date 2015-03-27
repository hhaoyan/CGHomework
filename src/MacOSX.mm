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
    // FIXME: I have no knowledge of Objective-C memory management,
    // so this may have some memory leak issues.
    @autoreleasepool {
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
        
        // flip the image
        {
            char* tmp = new char[[rep pixelsHigh] * 4];
            char* parsed = reinterpret_cast<char*>(buffer);
            long height = [rep pixelsHigh];
            long width = [rep pixelsWide];
            
            for(int i = 0;i<height / 2;i++){
                memcpy(tmp, &parsed[i * width * 4], width * 4);
                memcpy(&parsed[i * width * 4], &parsed[(height - 1 - i) * width * 4], width * 4);
                memcpy(&parsed[(height - i - 1) * width * 4], tmp, width * 4);
            }
            
            delete [] tmp;
        }
        
        *data = buffer;
        *w = [rep pixelsWide];
        *h = [rep pixelsHigh];
        *bpp = [rep bitsPerPixel];
    
        fn = nil;
        raw = nil;
        rep = nil;
    }
    return 1;
}

void FreeDecodedImage(void** data){
    free(*data);
    *data = NULL;
}