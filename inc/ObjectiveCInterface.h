//
//  ObjectiveCInterface.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/24/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_ObjectiveCInterface_h
#define libOOGL_ObjectiveCInterface_h

#ifdef __APPLE__

int DecodeImageFile(const char* filename, void** data, long* width, long* height, long* bitsPerPixel);
void FreeDecodedImage(void** data);

#elif defined WIN32

int DecodeImageFile(const char* filename, void** data, long* width, long* height, long* bitsPerPixel);
void FreeDecodedImage(void** data);

#else

#error "This header defines platform dependent routines for loading images, please implement for your platform first."

#endif

#endif
