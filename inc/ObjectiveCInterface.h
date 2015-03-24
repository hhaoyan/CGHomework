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

int DecodeImageFile(const char* filename, void** data, long* width, long* height, long* bytesPerPixel);
void FreeDecodedImage(void** data);

#endif

#endif
