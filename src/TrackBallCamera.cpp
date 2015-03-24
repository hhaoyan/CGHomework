//
//  TrackBallCamera.cpp
//  OpenGLFirst
//
//  Created by Haoyan Huo on 3/19/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "TrackBallCamera.h"

const float SMOOTH_FACTOR = 5.0f;
const float DYNAMIC_FACTOR = -0.4f;

TrackBallCamera::TrackBallCamera(GLApplication* app, float radius, float nearPlane, float farPlane, float fov):
    fApplication(app), fVectorLookat(0.0f, 0.0f, 0.0f), fVectorUp(0.0f, 1.0f, 0.0f),
    fVectorEye(0.0f, 0.0f, radius), fNeedsUpdate(true),
    fRotateBegin(1.0f, 0.0f, 0.0f), fRotateEnd(1.0f, 0.0f, 0.0f),
    fZoomFactor(1.0f), fNearPlane(nearPlane), fFarPlane(farPlane), fFov(fov),
    fTracking(false){
    
}

void TrackBallCamera::OnWindowSizeChanged(int width, int height, float nearPlane, float farPlane, float fov){
    fWindowSize[0] = width;
    fWindowSize[1] = height;
    
    if(nearPlane > 0)
        fNearPlane = nearPlane;
    if(farPlane > 0)
        fFarPlane = farPlane;
    if(fov > 0)
        fFov = fov;
    
    fViewportRatio = ((float)width)/height;
    fNeedsUpdate = true;
}

glm::vec3& TrackBallCamera::GetEyePosition(){
    return fVectorEye;
}

glm::mat4& TrackBallCamera::GetProjectionViewMatrix(){
    if(fNeedsUpdate){
        fViewMatrix = glm::lookAt(fVectorEye, fVectorLookat, fVectorUp);
        fProjectionMatrix = glm::perspective(fFov, fViewportRatio, fNearPlane, fFarPlane);
        fVPMatrix = fProjectionMatrix * fViewMatrix;
        fNeedsUpdate = false;
    }
    
    return fVPMatrix;
}

glm::quat quatFromAxisAndAngle(glm::vec3& axis, float angle){
    float half_angle = angle / 2;
    float s = sin(half_angle);
    
    return glm::quat(cos(half_angle), axis.x * s, axis.y * s, axis.z * s);
}

glm::vec3 GetMousePositionOnTrackBall(int x, int y, int width, int height,
                                      glm::vec3& up, glm::vec3 eye){
    int radius = (width + height) / 4;
    glm::vec3 ray((float)(x - width / 2) / radius, (float)(height / 2 - y) / radius, 0);
    if(glm::length(ray) > 1.0f){
        ray /= glm::length(ray);
    }
    else{
        ray.z = glm::sqrt((float)1 - glm::length(ray) * glm::length(ray));
    }
    assert(glm::abs(glm::length(ray) - 1.0f) < 0.005f);
    
    glm::vec3 component, caster;
    component = up;
    component = glm::normalize(component);
    component *= ray.y;
    
    caster += component;
    
    component = glm::cross(up, eye);
    component = glm::normalize(component);
    component *= ray.x;
    
    caster += component;
    
    component = eye;
    component = glm::normalize(component);
    component *= ray.z;
    
    caster += component;
    
    return caster;
}

void TrackBallCamera::OnFrameUpdate(){
    // TODO: handle the scroll events
    if(fApplication->GetWindowWidth() != fWindowSize[0] ||
       fApplication->GetWindowHeight() != fWindowSize[1]){
        OnWindowSizeChanged(fApplication->GetWindowWidth(), fApplication->GetWindowHeight());
    }
    
    if(fApplication->IsMouseKeyDown(0)){
        fRotateEnd = GetMousePositionOnTrackBall(fApplication->GetPointerX(), fApplication->GetPointerY(), fWindowSize[0], fWindowSize[1], fVectorUp, fVectorEye);
        
        if(!fTracking){
            fRotateBegin = fRotateEnd;
            fTracking = true;
        }
    } else {
        fTracking = false;
    }
    
    float cosd =
        glm::dot(fRotateBegin, fRotateEnd) / glm::length(fRotateBegin) / glm::length(fRotateEnd);
    if(cosd > 1.0f)
        cosd = 1.0f;
    
    float angle = glm::acos(cosd);
    
    if(angle > 0.0001f){
        angle /= SMOOTH_FACTOR;
        glm::vec3 axis = glm::cross(fRotateBegin, fRotateEnd);
        axis = glm::normalize(axis);
        
        glm::quat b = quatFromAxisAndAngle(axis, -angle);
        
        fVectorEye = glm::rotate(b,  fVectorEye);
        fVectorUp = glm::rotate(b, fVectorUp);
        
        fRotateEnd = glm::rotate(b, fRotateEnd);
        
        b = quatFromAxisAndAngle(axis, angle * DYNAMIC_FACTOR);
        fRotateBegin = glm::rotate(b, fRotateBegin);
        fNeedsUpdate = true;
    }
    if(fZoomFactor != 1.0f){
        fVectorEye *= (fZoomFactor - 1.0f) / SMOOTH_FACTOR / 3.0f + 1.0f;
        fZoomFactor /= (fZoomFactor - 1.0f) / SMOOTH_FACTOR / 3.0f + 1.0f;
        if(abs(fZoomFactor - 1.0f) <= 0.00001f)
            fZoomFactor = 1.0f;
        fNeedsUpdate = true;
    }
}
