//
//  TrackBallCamera.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __libOOGL__TrackBallCamera__
#define __libOOGL__TrackBallCamera__

#include "gl.h"
#include "GLApplication.h"

class TrackBallCamera{
public:
    TrackBallCamera(GLApplication* app, float radius, float nearPlane, float farPlane, float fov);
    void OnFrameUpdate();
    // set near and far plane to -1 to keep last value
    void OnWindowSizeChanged(int width, int height,
                             float nearPlane=-1.0f, float farPlane=-1.0f, float fov=-1.0f);
    
    glm::mat4& GetProjectionViewMatrix();
    glm::vec3& GetEyePosition();
    
    void ZoomIn() {fZoomFactor *= 0.5;}
    void ZoomOut() {fZoomFactor *= 1.5;}
protected:
    GLApplication*  fApplication;
    
    int       fWindowSize[2];
    float			fZoomFactor;
    glm::vec3		fRotateBegin, fRotateEnd;
    glm::mat4		fViewMatrix, fProjectionMatrix, fVPMatrix;
    glm::vec3		fVectorEye, fVectorUp, fVectorLookat;
    float			fFov, fViewportRatio, fNearPlane, fFarPlane;
    bool            fTracking;
    bool			fNeedsUpdate;
};

#endif /* defined(__OpenGLFirst__TrackBallCamera__) */
