//
//  FilmInterference.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/29/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef libOOGL_FilmInterference_h
#define libOOGL_FilmInterference_h

#include "GLApplication.h"
#include "GLTextRenderManager.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "TrackBallCamera.h"
#include "GLSkybox.h"
#include "AssetManager.h"
#include <fstream>

extern "C"{
    struct colourSystem {
        char *name;     	    	    /* Colour system name */
        double xRed, yRed,	    	    /* Red x, y */
        xGreen, yGreen,  	    /* Green x, y */
        xBlue, yBlue,    	    /* Blue x, y */
        xWhite, yWhite,  	    /* White point x, y */
        gamma;   	    	    /* Gamma correction for system */
    } EBUsystem =  { "CIE",                0.7355, 0.2645, 0.2658, 0.7243, 0.1669, 0.0085,0.33333333, 0.33333333,0};
    
    void spectrum_to_xyz(double (*spec_intens)(double wavelength),
                         double *x, double *y, double *z);
    void xyz_to_rgb(struct colourSystem *cs,
                    double xc, double yc, double zc,
                    double *r, double *g, double *b);
    int constrain_rgb(double *r, double *g, double *b);
    void norm_rgb(double *r, double *g, double *b);
    
}
namespace CIE {
    double n = 1.5;
    double w = 500;
    const double PI = 3.14159265;
    double theta = PI / 3;
    
    double bodyTemp = 5778;
    
    double film_interference(double wavelength)
    {
        double s = sin(2 * PI  * w * n * cos(theta) / wavelength);
        
        //if(wavelength - 2.5 < 632.8 && wavelength + 2.5 > 632.8)
          //  return s* s;
        //else
          //  return 0;
        
        // linear spectrum
        // return s*s;
        
        // black body radiation
        double wlm = wavelength * 1e-9;   /* Wavelength in meters */
        double bb = (3.74183e-16 * pow(wlm, -5.0)) /
            (exp(1.4388e-2 / (wlm * bodyTemp)) - 1.0) * s * s;
        return bb;
        
        // rayleigh scattering
        double scattering = (wavelength / 1000);
        scattering = scattering * scattering * scattering * scattering;
        bb *= scattering;
        
        return bb;
    }
    
    void prepare_interference_texture_bgr888(void* buffer, int width, int height, int min_thickness, int max_thickness)
    {
        struct colourSystem *cs = &EBUsystem;
        
        // x: theta
        // y: thickness (light distance)
        unsigned char* rgb = reinterpret_cast<unsigned char*>(buffer);
        
        for(int i = 0;i<height;++i){
            for(int j = 0;j<width;++j){
                unsigned char *rgb_i_j = rgb + i * width * 3 + j * 3;
                
                double thickness = min_thickness + (max_thickness - min_thickness) * i / height;
                theta = PI / 2 * j / width;
                w = thickness;
                
                double x,y,z,r,g,b;
                
                spectrum_to_xyz(film_interference, &x, &y, &z);
                xyz_to_rgb(cs, x, y, z, &r, &g, &b);
                
                if (constrain_rgb(&r, &g, &b)) {
                    norm_rgb(&r, &g, &b);
                } else {
                    norm_rgb(&r, &g, &b);
                }
                
                rgb_i_j[0] = int(b * 255);
                rgb_i_j[1] = int(g * 255);
                rgb_i_j[2] = int(r * 255);
            }
        }
    }
}

namespace BITMAP {
    typedef unsigned int LONG;
    typedef unsigned int DWORD;
    typedef unsigned short WORD;
#pragma pack(push,1)
    struct bmpHeader{
        WORD	bfType;
        DWORD	bFilesize;
        WORD	bfR1;
        WORD	bfR2;
        DWORD	bfOffsetToData;
    };
    
    struct bmpInfo{
        DWORD	biSize;
        LONG	biWidth, biHeight;
        WORD	biPlanes;
        WORD	biBitCount;
        DWORD	biCompression;
        DWORD	biSizeImage;
        LONG	ppm[2];
        DWORD	biclrs[2];
    };
#pragma pack(pop)
    
    void SaveBitmapFile(const char* name, void* buf, int width, int height){
        int bufSize = width * height * 3;
        
        bmpHeader hdr;
        
        hdr.bfType = 0x4d42;
        hdr.bFilesize = sizeof(bmpHeader) + sizeof(bmpInfo) + bufSize;
        hdr.bfR1 = hdr.bfR2 = 0;
        hdr.bfOffsetToData = sizeof(bmpHeader) + sizeof(bmpInfo);
        
        bmpInfo info;
        
        info.biSize = sizeof(bmpInfo);
        info.biWidth = width;
        info.biHeight = height;
        info.biPlanes = 1;
        info.biBitCount = 24;
        info.biCompression = 0;
        info.biSizeImage = 0;
        info.ppm[0] = info.ppm[1] = 0;
        info.biclrs[0] = info.biclrs[1] = 0;
        
        std::ofstream fout(name);
        if(fout.fail())
            return;
        
        fout.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
        fout.write(reinterpret_cast<const char*>(&info), sizeof(info));
        fout.write(reinterpret_cast<const char*>(buf), bufSize);
        fout.close();
    }
}


class FilmInterference : public GLApplication{
    TrackBallCamera* camera;
    GLTexture* fInterferenceTex;
    GLCubemapTexture* skytex;
    GLSkybox * sky;
    GLShader* fshader;
    GLMesh* sphere;
public:
    virtual void MouseScroll(double x, double y){
        if(y > 0)
            camera->ZoomOut();
        else
            camera->ZoomIn();
    }
    virtual void CreateApplication(){
        camera = new TrackBallCamera(this, 5.0f, 0.1f, 100.0f, 45.0);
        
        {
            /*fInterferenceTex = new GLTexture();
            fInterferenceTex->SetTextureSize(1024, 1024, GLTexture::kBGR888);
            void* buf;
            fInterferenceTex->GetTextureBuffer(&buf, NULL);
            
            Info(__FUNCTION__, "preparing interference lookup texture...");
            
            CIE::prepare_interference_texture_bgr888(buf, 1024, 1024, 300, 1300);
            BITMAP::SaveBitmapFile(AssetManager::GetAsset("interference.bmp"), buf, 1024, 1024);
            
            Info(__FUNCTION__, "interference lookup texture done.");*/
            
            fInterferenceTex = GLTexture::LoadFromFile(AssetManager::GetAsset("interference.bmp"));
            fInterferenceTex->Attach();
            
        }
        {
            skytex = GLCubemapTexture::LoadFromFiles(AssetManager::GetAsset("/Park2/"),
                                                     "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg");
            skytex->Attach();
            sky = new GLSkybox(skytex);
            sky->Attach();
            
            sphere = GLMesh::SimpleMeshGenerator::Sphere(GLMesh::kPosTexNor, 1.0f, 50, 50);
            //sphere = GLMesh::SimpleMeshGenerator::Cuboid(GLMesh::kPosTexNor, 1.0f, 1.0f, 1.0f);
            sphere->Attach();
            
            fshader = new GLShader(AssetManager::GetAsset("fresnel_with_interference.shader"), "cameraPosition\nmRefractionRatio\nmFresnelBias\nmFresnelScale\nmFresnelPower\nmvpMatrix\nmodelMatrix\ntCube\ntInterference");
            fshader->Attach();
        }
        
        {
            // font and error check
            GLTextRenderManager::SetFontname(AssetManager::GetAsset("fonts/Consolas.ttf"));
            glClearColor(0.4f, 0.5f, 0.66f, 1.0f);
            
            glEnable(GL_DEPTH_TEST);
            OpenGLShouldHaveNoError(__FUNCTION__);
        }
    }
    
    virtual void RenderFrame(){
        camera->OnFrameUpdate();
        glm::mat4 mvp = camera->GetProjectionViewMatrix();
        glm::vec3 cameraPosition = camera->GetEyePosition();
        glm::mat4 ident = glm::translate(glm::vec3(0, 0, 0));
        
        glViewport(0, 0, GetWindowFramebufferWidth(), GetWindowFramebufferHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky->RenderSkybox(mvp, cameraPosition);
        
        fshader->UseProgram();
        
        fshader->Uniform1f("mRefractionRatio", 1.02);
        fshader->Uniform1f("mFresnelBias", 0.1); // original 0.1
        fshader->Uniform1f("mFresnelPower", 2.0);
        fshader->Uniform1f("mFresnelScale", 1.0);
        fshader->Uniform3fv("cameraPosition", 1, &cameraPosition[0]);
        fshader->UniformMatrix4fv("mvpMatrix", 1, GL_FALSE, &mvp[0][0]);
        fshader->UniformMatrix4fv("modelMatrix", 1, GL_FALSE, &ident[0][0]);
        fshader->Uniform1i("tCube", 0);
        fshader->Uniform1i("tInterference", 1);
        
        skytex->ActivateAndBindTexture(GL_TEXTURE0);
        fInterferenceTex->ActivateAndBindTexture(GL_TEXTURE1);
        
        sphere->UseMeshAndDrawTriangles();
        
        float sx = 2.0 / GetWindowFramebufferWidth();
        float sy = 2.0 / GetWindowFramebufferHeight();
        char buf[100];
        static int counter = -1;
        
        sprintf(buf, "Frame: %.4d", ++counter);
        glm::vec3 color(1.0f, 0.7f, 0.9f);
        GLTextRenderManager::AddText(NULL, buf, 50, -1 + sx * 50, 1 - 100 * sy, sx, sy,
                                     color);
        
        
        GLTextRenderManager::RenderText();
    }
    
    virtual void ShutdownApplication(){
        
    }
};

#endif
