//
//  GLMesh.h
//  libOOGL
//
//  Created by Haoyan Huo on 3/20/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#ifndef __libOOGL__GLMesh__
#define __libOOGL__GLMesh__

#include "gl.h"
#include "GLObject.h"
#include "GLAttachable.h"

class GLMesh : public GLObject, public GLAttachable{
public:
    // masks indicate what components should mesh have
    enum MeshComponents{
        kPosition = 1,
        kTextureCoord = 2,
        kNormal = 4,
        kTangent = 8,
        kBitangent = 16,
        
        kPosTex = kPosition|kTextureCoord,
        kPosNor = kPosition|kNormal,
        kPosTexNor = kPosTex|kNormal,
        kPosTexNorTan = kPosTexNor | kTangent,
        kPosTexNorTanBi = kPosTexNorTan | kBitangent,
    };
    
    static GLMesh* LoadFromObjFile(const char* filename, MeshComponents mode);
    
    // Useful simple mesh.
    class SimpleMeshGenerator{
    public:
        static GLMesh* Cuboid(MeshComponents mode, float width, float height, float depth);
        static GLMesh* Sphere(MeshComponents mode, float radius, int segV, int segH);
        static GLMesh* Plane(MeshComponents mode, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
        static GLMesh* PlaneXY(MeshComponents mode, float widthOfX, float widthOfY){
            return Plane(mode,
                         glm::vec3(-widthOfX / 2, widthOfY / 2, 0.0f),
                         glm::vec3(widthOfX / 2, widthOfY / 2, 0.0f),
                         glm::vec3(-widthOfX / 2, -widthOfY / 2, 0.0f),
                         glm::vec3(widthOfX / 2, -widthOfY / 2, 0.0f));
        }
        static GLMesh* PlaneYZ(MeshComponents mode, float widthOfY, float widthOfZ){
            return Plane(mode,
                         glm::vec3(0.0f, widthOfY / 2, widthOfZ / 2),
                         glm::vec3(0.0f, widthOfY / 2, -widthOfZ / 2),
                         glm::vec3(0.0f, -widthOfY / 2, widthOfZ / 2),
                         glm::vec3(0.0f, -widthOfY / 2, -widthOfZ / 2));
        }
        static GLMesh* PlaneZX(MeshComponents mode, float widthOfX, float widthOfZ){
            return Plane(mode,
                         glm::vec3(-widthOfX / 2, 0.0f, -widthOfZ / 2),
                         glm::vec3(widthOfX / 2, 0.0f, -widthOfZ / 2),
                         glm::vec3(-widthOfX / 2, 0.0f, widthOfZ / 2),
                         glm::vec3(widthOfX / 2, 0.0f, widthOfZ / 2));
        }
    };
    
    class VertexDataProxy{
    public:
        VertexDataProxy(const VertexDataProxy& other) {fMesh = other.fMesh;}
        friend class GLMesh;
        
        void SetVertexCount(unsigned int count);
        unsigned int GetVertexCount();
        
        glm::vec3& Position(unsigned int index);
        glm::vec2& TextureCoord(unsigned int index);
        glm::vec3& Normal(unsigned int index);
        glm::vec3& Tangent(unsigned int index);
        glm::vec3& BiTangent(unsigned int index);
        
    private:
        GLMesh* fMesh;
        VertexDataProxy(GLMesh* mesh) {fMesh = mesh;}
    };
    
    class IndexDataProxy{
    public:
        IndexDataProxy(const IndexDataProxy& other) {fMesh = other.fMesh;}
        friend class GLMesh;
        
        void SetIndexCount(unsigned int count);
        unsigned int GetIndexCount();
        
        unsigned short& IndexAt(unsigned int index);
        
    private:
        GLMesh* fMesh;
        IndexDataProxy(GLMesh* mesh) {fMesh = mesh;}
    };
    
    friend class VertexDataProxy;
    friend class IndexDataProxy;
    
    GLMesh(MeshComponents comp);
    ~GLMesh();
    
    VertexDataProxy GetVertexProxy();
    IndexDataProxy GetIndexProxy();
    
    void ComputeNormals();
    
    virtual bool IsAttached();
    virtual void Attach();
    virtual void Detach();
    
    GLuint GetVertexCount();
    GLuint GetIndexCount();
    GLuint GetVertexBufferObject();
    GLuint GetIndexBufferObject();
    GLuint GetVertexArrayObject();
    
    void UseMesh();
    void UseMeshAndDrawTriangles();
    
protected:
    virtual void DeleteObject();
private:
    GLuint      fVertexBufferObject, fIndexBufferObject, fVertexArrayObject;
    bool        fAttached;
    void       *fVertexBuffer, *fIndexBuffer;
    unsigned int fVertexCount, fIndexCount,
                fSizePerVertex;
    MeshComponents fComp;
};

#endif /* defined(__OpenGLFirst__GLMesh__) */
