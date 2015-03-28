//
//  GLMesh.cpp
//  libOOGL
//
//  Created by Haoyan Huo on 3/20/15.
//  Copyright (c) 2015 Haoyan Huo. All rights reserved.
//

#include "GLMesh.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>

/**
 * @brief The registry for all vertex arrays
 */
struct VertexArrayRef{
    GLuint fVertexArrayObject;
    int fUsageCount;
};
std::unordered_map<int, VertexArrayRef> gVertexArrays;

bool IsVAOExistAndGetVAO(GLMesh::MeshComponents mode, GLuint* fVAO){
    int key = static_cast<int>(mode);
    
    auto item = gVertexArrays.find(key);
    if(item == gVertexArrays.end()){
        return false;
    } else {
        item->second.fUsageCount += 1;
        *fVAO = item->second.fVertexArrayObject;
        return true;
    }
}

void RegisterNewVAO(GLMesh::MeshComponents mode, GLuint fVAO){
    // mode should never exist in gVA!
    VertexArrayRef ref;
    ref.fUsageCount = 1;
    ref.fVertexArrayObject = fVAO;
    gVertexArrays[static_cast<int>(mode)] = ref;
}

bool FreeVAOAndIfNeedsDestruct(GLMesh::MeshComponents mode){
    auto item = gVertexArrays.find(static_cast<int>(mode));
    
    if(item != gVertexArrays.end()){
        item->second.fUsageCount -= 1;
        if(item->second.fUsageCount <= 0){
            gVertexArrays.erase(item);
            return true;
        }
    }
    
    return false;
}

GLMesh::GLMesh(GLMesh::MeshComponents comp):
    fVertexBuffer(0), fIndexBuffer(0), fVertexCount(0), fIndexCount(0),
    fSizePerVertex(0), fComp(comp), fAttached(false),
    fVertexArrayObject(0), fVertexBufferObject(0), fIndexBufferObject(0){
    
}

GLuint GLMesh::GetVertexCount(){
    return fVertexCount;
}

GLuint GLMesh::GetIndexCount(){
    return fIndexCount;
}

GLuint GLMesh::GetVertexArrayObject(){
    if(!IsAttached())
        Error(__FUNCTION__, "GLMesh not attached");
    
    return fVertexArrayObject;
}

GLuint GLMesh::GetVertexBufferObject(){
    if(!IsAttached())
        Error(__FUNCTION__, "GLMesh not attached");
    
    return fVertexBufferObject;
}

GLuint GLMesh::GetIndexBufferObject(){
    if(!IsAttached())
        Error(__FUNCTION__, "GLMesh not attached");
    
    return fIndexBufferObject;
}

void GLMesh::UseMesh(){
    if(!IsAttached())
        Error(__FUNCTION__, "GLMesh not attached");
    
    glBindVertexArray(fVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, fVertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fIndexBufferObject);
}

void GLMesh::UseMeshAndDrawTriangles(){
    // TODO: should we auto-attach?
    if(!IsAttached())
        Error(__FUNCTION__, "GLMesh not attached");
    
    UseMesh();
    glDrawElements(GL_TRIANGLES, fIndexCount, GL_UNSIGNED_SHORT, 0);
    OpenGLShouldHaveNoError(__FUNCTION__);
}

void GLMesh::Detach(){
    if(!IsAttached())
        return;
    
    glDeleteBuffers(1, &fVertexBufferObject);
    glDeleteBuffers(1, &fIndexBufferObject);
    if(FreeVAOAndIfNeedsDestruct(fComp))
        glDeleteVertexArrays(1, &fVertexArrayObject);
    
    OpenGLShouldHaveNoError(__FUNCTION__);
    
    fAttached = false;
}
void GLMesh::Attach(){
    if(IsAttached())
        return;
    
    if(!fVertexBuffer || !fIndexBuffer)
        Error(__FUNCTION__, "Not enough buffer to attach");
    
    glGenBuffers(1, &fVertexBufferObject);
    glGenBuffers(1, &fIndexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, fVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, fSizePerVertex * fVertexCount, fVertexBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * fIndexCount, fIndexBuffer, GL_STATIC_DRAW);
    
    if(!IsVAOExistAndGetVAO(fComp, &fVertexArrayObject)){
        glGenVertexArrays(1, &fVertexArrayObject);
        glBindVertexArray(fVertexArrayObject);
    
        GLuint id = 0;
        int offset = 0;
        if(fComp & kPosition){
            glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, fSizePerVertex, reinterpret_cast<GLvoid*>(offset));
            id += 1;
            offset += sizeof(glm::vec3);
        }
        if(fComp & kTextureCoord){
            glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, fSizePerVertex, reinterpret_cast<GLvoid*>(offset));
            id += 1;
            offset += sizeof(glm::vec2);
        }
        if(fComp & kNormal){
            glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, fSizePerVertex, reinterpret_cast<GLvoid*>(offset));
            id += 1;
            offset += sizeof(glm::vec3);
        }
        if(fComp & kTangent){
            glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, fSizePerVertex, reinterpret_cast<GLvoid*>(offset));
            id += 1;
            offset += sizeof(glm::vec3);
        }
        if(fComp & kBitangent){
            glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, fSizePerVertex, reinterpret_cast<GLvoid*>(offset));
            id += 1;
            offset += sizeof(glm::vec3);
        }
    
        for(GLuint i = 0;i<id;++i)
            glEnableVertexAttribArray(i);
    
        glBindVertexArray(0);
        OpenGLShouldHaveNoError(__FUNCTION__);
        
        RegisterNewVAO(fComp, fVertexArrayObject);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    OpenGLShouldHaveNoError(__FUNCTION__);
    
    fAttached = true;
}

bool GLMesh::IsAttached(){
    return fAttached;
}

GLMesh::~GLMesh(){
    if(fVertexBuffer != NULL)
        free(fVertexBuffer);
    
    if(fIndexBuffer != NULL)
        free(fIndexBuffer);
    
    if(IsAttached())
        Warning(__FUNCTION__, "GLMesh at %p not detached", reinterpret_cast<void*>(this));
}

void GLMesh::DeleteObject(){
    if(IsAttached())
        Detach();
}

void GLMesh::ComputeNormals(){
    if(!fVertexBuffer || !fIndexBuffer || (fComp & kNormal) == 0){
        Error(__FUNCTION__, "compute normals: buffer not ready or no normal component");
    }
    
    IndexDataProxy iproxy(GetIndexProxy());
    VertexDataProxy vproxy(GetVertexProxy());
    unsigned short s0, s1, s2;
    
    for(int i = 0;i<iproxy.GetIndexCount();i+= 3){
        s0 = iproxy.IndexAt(i);
        s1 = iproxy.IndexAt(i+1);
        s2 = iproxy.IndexAt(i+2);
        
        glm::vec3
            &v0 = vproxy.Position(s0),
            &v1 = vproxy.Position(s1),
            &v2 = vproxy.Position(s2);
        
        glm::vec3 a0, a1;
        a0 = v0 - v1;
        a1 = v1 - v2;
        
        glm::vec3 normal = glm::cross(a0, a1);
        normal = glm::normalize(normal);
        
        vproxy.Normal(s0) = normal;
        vproxy.Normal(s1) = normal;
        vproxy.Normal(s2) = normal;
    }
}

GLMesh::VertexDataProxy GLMesh::GetVertexProxy(){
    VertexDataProxy proxy(this);
    return proxy;
}

GLMesh::IndexDataProxy GLMesh::GetIndexProxy(){
    IndexDataProxy proxy(this);
    return proxy;
}

void GLMesh::VertexDataProxy::SetVertexCount(unsigned int count){
    if (fMesh->fVertexBuffer) {
        fMesh->Error(__FUNCTION__, "Resizing vertex buffer is not allowed");
    }
    
    if(count >= (unsigned short)0xffffffff){
        fMesh->Warning(__FUNCTION__, "vertex count %d >= MAX_SHORT", count);
    }
    
    fMesh->fSizePerVertex = 0;
    if(fMesh->fComp & kPosition)
        fMesh->fSizePerVertex += sizeof(glm::vec3);
    if(fMesh->fComp & kTextureCoord)
        fMesh->fSizePerVertex += sizeof(glm::vec2);
    if(fMesh->fComp & kNormal)
        fMesh->fSizePerVertex += sizeof(glm::vec3);
    if(fMesh->fComp & kTangent)
        fMesh->fSizePerVertex += sizeof(glm::vec3);
    if(fMesh->fComp & kBitangent)
        fMesh->fSizePerVertex += sizeof(glm::vec3);
        
    fMesh->fVertexBuffer = malloc(fMesh->fSizePerVertex * count);
    if(!fMesh->fVertexBuffer)
        fMesh->Error(__FUNCTION__, "allocate vertex buffer failed, out of memory");
    
    fMesh->fVertexCount = count;
}

unsigned int GLMesh::VertexDataProxy::GetVertexCount(){
    return fMesh->fVertexCount;
}

glm::vec3& GLMesh::VertexDataProxy::Position(unsigned int index){
    if(index >= fMesh->fVertexCount || (fMesh->fComp & kPosition) == 0)
        fMesh->Error(__FUNCTION__, "out of bound when accessing position data");
    
    unsigned char *bytes = static_cast<unsigned char*>(fMesh->fVertexBuffer);
    void* buf = static_cast<void*>(bytes + index * fMesh->fSizePerVertex);
    return *static_cast<glm::vec3*>(buf);
}

glm::vec2& GLMesh::VertexDataProxy::TextureCoord(unsigned int index){
    if(index >= fMesh->fVertexCount || (fMesh->fComp & kTextureCoord) == 0)
        fMesh->Error(__FUNCTION__, "out of bound when accessing texture coordinate data");
    
    unsigned char* bytes = static_cast<unsigned char*>(fMesh->fVertexBuffer);
    
    unsigned int jmp = index * fMesh->fSizePerVertex;
    if(fMesh->fComp & kPosition)
        jmp += sizeof(glm::vec3);
    
    void *buf = static_cast<void*>(bytes + jmp);
    return *static_cast<glm::vec2*>(buf);
}

glm::vec3& GLMesh::VertexDataProxy::Normal(unsigned int index){
    if(index >= fMesh->fVertexCount || (fMesh->fComp & kNormal) == 0)
        fMesh->Error(__FUNCTION__, "out of bound when accessing normal data");
    
    unsigned char* bytes = static_cast<unsigned char*>(fMesh->fVertexBuffer);
    
    unsigned int jmp = index * fMesh->fSizePerVertex;
    if(fMesh->fComp & kPosition)
        jmp += sizeof(glm::vec3);
    if(fMesh->fComp & kTextureCoord)
        jmp += sizeof(glm::vec2);
    
    void *buf = static_cast<void*>(bytes + jmp);
    return *static_cast<glm::vec3*>(buf);
}

glm::vec3& GLMesh::VertexDataProxy::Tangent(unsigned int index){
    if(index >= fMesh->fVertexCount || (fMesh->fComp & kTangent) == 0)
        fMesh->Error(__FUNCTION__, "out of bound when accessing tangent data");
    
    unsigned char* bytes = static_cast<unsigned char*>(fMesh->fVertexBuffer);
    
    unsigned int jmp = index * fMesh->fSizePerVertex;
    if(fMesh->fComp & kPosition)
        jmp += sizeof(glm::vec3);
    if(fMesh->fComp & kTextureCoord)
        jmp += sizeof(glm::vec2);
    if(fMesh->fComp & kNormal)
        jmp += sizeof(glm::vec3);
    
    void *buf = static_cast<void*>(bytes + jmp);
    return *static_cast<glm::vec3*>(buf);
}

glm::vec3& GLMesh::VertexDataProxy::BiTangent(unsigned int index){
    if(index >= fMesh->fVertexCount || (fMesh->fComp & kBitangent) == 0)
        fMesh->Error(__FUNCTION__, "out of bound when accessing tangent data");
    
    unsigned char* bytes = static_cast<unsigned char*>(fMesh->fVertexBuffer);
    
    unsigned int jmp = index * fMesh->fSizePerVertex;
    if(fMesh->fComp & kPosition)
        jmp += sizeof(glm::vec3);
    if(fMesh->fComp & kTextureCoord)
        jmp += sizeof(glm::vec2);
    if(fMesh->fComp & kNormal)
        jmp += sizeof(glm::vec3);
    if(fMesh->fComp & kBitangent)
        jmp += sizeof(glm::vec3);
    
    void *buf = static_cast<void*>(bytes + jmp);
    return *static_cast<glm::vec3*>(buf);
}

void GLMesh::IndexDataProxy::SetIndexCount(unsigned int count){
    if (fMesh->fIndexBuffer) {
        fMesh->Error(__FUNCTION__, "Resizing index buffer is not allowed");
    }
    
    fMesh->fIndexBuffer = malloc(sizeof(unsigned short) * count);
    if(!fMesh->fIndexBuffer)
        fMesh->Error(__FUNCTION__, "allocate index buffer failed, out of memory");
    
    fMesh->fIndexCount = count;
}

unsigned int GLMesh::IndexDataProxy::GetIndexCount(){
    return fMesh->fIndexCount;
}

unsigned short& GLMesh::IndexDataProxy::IndexAt(unsigned int index){
    if(index >= fMesh->fIndexCount)
        fMesh->Error(__FUNCTION__, "out of bound when accessing index data");
    
    unsigned char* bytes = static_cast<unsigned char*>(fMesh->fIndexBuffer);
    void * buf = static_cast<void*>(bytes + index * sizeof(unsigned short));
    return *static_cast<unsigned short*>(buf);
}

void AddPlaneToMesh(GLMesh::VertexDataProxy& vproxy, GLMesh::IndexDataProxy& iproxy, int vstart, int istart, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, bool texcoord=true){
    
    static glm::vec2 texs[4] = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f)
    };
    
    vproxy.Position(vstart) = p0;
    vproxy.Position(vstart+1) = p1;
    vproxy.Position(vstart+2) = p2;
    vproxy.Position(vstart+3) = p3;
    if(texcoord){
        vproxy.TextureCoord(vstart) = texs[0];
        vproxy.TextureCoord(vstart+1) = texs[1];
        vproxy.TextureCoord(vstart+2) = texs[2];
        vproxy.TextureCoord(vstart+3) = texs[3];
    }
    
    static unsigned short inxs[6] = {
        0, 2, 1, 1, 2, 3
    };
    
    for (int i=0; i<6; ++i) {
        iproxy.IndexAt(istart+i) = inxs[i] + vstart;
    }
}

GLMesh* GLMesh::SimpleMeshGenerator::Plane(GLMesh::MeshComponents mode, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    
    GLMesh* mesh = new GLMesh(mode);
    
    if(mode != (kPosition | kTextureCoord | kNormal)){
        mesh->Error(__FUNCTION__, "Plane generator for kTangent|kBitangent not implemented");
    }

    VertexDataProxy vproxy(mesh->GetVertexProxy());
    IndexDataProxy iproxy(mesh->GetIndexProxy());
    vproxy.SetVertexCount(4);
    iproxy.SetIndexCount(6);
    
    AddPlaneToMesh(vproxy, iproxy, 0, 0, p0, p1, p2, p3);
    
    mesh->ComputeNormals();
    return mesh;
}

GLMesh* GLMesh::SimpleMeshGenerator::Cuboid(GLMesh::MeshComponents mode, float x, float y, float z){
    GLMesh* mesh = new GLMesh(mode);
    
    if((mode & (~kPosition) & (~kTextureCoord) & (~kNormal))){
        mesh->Error(__FUNCTION__, "Plane generator for kTangent|kBitangent not implemented");
    }
    
    VertexDataProxy vproxy(mesh->GetVertexProxy());
    IndexDataProxy iproxy(mesh->GetIndexProxy());
    
    vproxy.SetVertexCount(24);
    iproxy.SetIndexCount(36);
    
    AddPlaneToMesh(vproxy, iproxy, 0, 0,
                   glm::vec3(-x / 2.0f, y / 2.0f, z / 2.0f),
                   glm::vec3(x / 2.0f, y / 2.0f, z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, z/ 2.0f),
                   glm::vec3(x / 2.0f, -y / 2.0f, z / 2.0f),
                   (mode & kTextureCoord) != 0);
    
    AddPlaneToMesh(vproxy, iproxy, 4, 6,
                   glm::vec3(x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(x / 2.0f, -y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, -z/ 2.0f),
                   (mode & kTextureCoord) != 0);
    
    AddPlaneToMesh(vproxy, iproxy, 8, 12,
                   glm::vec3(-x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, y / 2.0f, z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, z / 2.0f),
                   (mode & kTextureCoord) != 0);
    
    AddPlaneToMesh(vproxy, iproxy, 12, 18,
                   glm::vec3(x / 2.0f, y / 2.0f, z / 2.0f),
                   glm::vec3(x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(x / 2.0f, -y / 2.0f, z / 2.0f),
                   glm::vec3(x / 2.0f, -y / 2.0f, -z / 2.0f),
                   (mode & kTextureCoord) != 0);
    
    AddPlaneToMesh(vproxy, iproxy, 16, 24,
                   glm::vec3(-x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(x / 2.0f, y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, y / 2.0f, z / 2.0f),
                   glm::vec3(x / 2.0f, y / 2.0f, z / 2.0f),
                   (mode & kTextureCoord) != 0);
    
    AddPlaneToMesh(vproxy, iproxy, 20, 30,
                   glm::vec3(x / 2.0f, -y / 2.0f, -z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, -z / 2.0f),
                   glm::vec3(x / 2.0f, -y / 2.0f, z / 2.0f),
                   glm::vec3(-x / 2.0f, -y / 2.0f, z / 2.0f),
                   (mode & kTextureCoord) != 0);
    
    if((mode & kNormal) != 0)
        mesh->ComputeNormals();
    return mesh;
}

GLMesh* GLMesh::SimpleMeshGenerator::Sphere(GLMesh::MeshComponents mode, float radius, int segV, int segH){
    GLMesh *mesh = new GLMesh(mode);
    
    if(mode != (kPosition | kTextureCoord | kNormal)){
        mesh->Error(__FUNCTION__, "Plane generator for kTangent|kBitangent not implemented");
    }
    
    VertexDataProxy vproxy(mesh->GetVertexProxy());
    IndexDataProxy iproxy(mesh->GetIndexProxy());
    
    vproxy.SetVertexCount((segV + 1) * (segH + 1));
        
    for(int i = 0;i<=segV;i++){
        for(int j = 0;j<=segH;j++){
            float theta = glm::pi<float>() / segV * i;
            float alpha = glm::pi<float>() * 2 / segH * j;
            
            float x = -radius * glm::sin(theta) * glm::sin(alpha);
            float y = -radius * cos(theta);
            float z = -radius * sin(theta) * cos(alpha);

            float texH = (float)j / segH;
            float texV = (float)i / segV;

            unsigned int id = i * (segH + 1) + j;
            vproxy.Position(id) = glm::vec3(x, y, z);
            vproxy.TextureCoord(id) = glm::vec2(texH, texV);
            vproxy.Normal(id) = glm::normalize(glm::vec3(x, y, z));
        }
    }
    
    iproxy.SetIndexCount(segV * segH * 6);
    
    for(int i = 0;i<segV;i++){
        for(int j = 0;j<segH;j++){
            unsigned short base_index;
            base_index = i * (segH + 1) + j;
            
            unsigned int id = (i * segH + j) * 6;
            iproxy.IndexAt(id++) = base_index + 0;
            iproxy.IndexAt(id++) = base_index + segH + 1 + 1;
            iproxy.IndexAt(id++) = base_index + segH + 1;
            iproxy.IndexAt(id++) = base_index + 0;
            iproxy.IndexAt(id++) = base_index + 1;
            iproxy.IndexAt(id++) = base_index + segH + 1 + 1;
        }
    }
        

    return mesh;
}

GLMesh* GLMesh::LoadFromObjFile(const char *filename, GLMesh::MeshComponents mode){
    GLMesh *mesh = new GLMesh(mode);
    
    if(mode != kPosTexNor)
        mesh->Error(__FUNCTION__, "only kPosTexNor obj file could be imported");
    
    std::ifstream fin(filename);
    if(fin.fail())
        mesh->Error(__FUNCTION__, "open %s failed", filename);
    
    std::string line; line.reserve(64);
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned short> indices;
    
    GLMesh::VertexDataProxy vproxy(mesh->GetVertexProxy());
    GLMesh::IndexDataProxy iproxy(mesh->GetIndexProxy());
    
    bool isVCountSet = false;
    
    while (std::getline(fin, line, '\n')) {
        {
            // starting spaces...
            size_t item = line.find_first_not_of(" \r\n");
            if(item == std::string::npos)
                continue;
            line.erase(line.begin(), line.begin() + item);
            if(line.size() == 0)
                continue;
        }
        
        std::stringstream ss(line);
        float f0, f1, f2;
        unsigned short s0, s1, s2;
        unsigned char c;
        
        switch (line[0]) {
            case '#':
                break;
                
            case 'v':
                switch (line[1]) {
                    case 't':
                        ss>>c>>c>>f0>>f1;
                        texcoords.push_back(glm::vec2(f0, f1));
                        break;
                        
                    case 'n':
                        ss>>c>>c>>f0>>f1>>f2;
                        normals.push_back(glm::vec3(f0, f1, f2));
                        break;
                        
                    default:
                        ss>>c>>f0>>f1>>f2;
                        vertices.push_back(glm::vec3(f0, f1, f2));
                        break;
                }
                break;
                
            case 'f':
                if(!isVCountSet){
                    isVCountSet = true;
                    vproxy.SetVertexCount(static_cast<unsigned int>(vertices.size()));
                }
                ss>>c;
                for (int i = 0; i<3; ++i) {
                    ss>>s0>>c>>s1>>c>>s2;
                    s0 --;
                    s1 --;
                    s2 --;
                    vproxy.Position(s0) = vertices[s0];
                    vproxy.TextureCoord(s0) = texcoords[s1];
                    vproxy.Normal(s0) = normals[s2];
                    indices.push_back(s0);
                }
                break;
                
            default:
                break;
        }
        
    }
    iproxy.SetIndexCount(static_cast<unsigned int>(indices.size()));
    for (int i = 0; i<indices.size(); ++i) {
        iproxy.IndexAt(i) = indices[i];
    }
    
    fin.close();
    
    return mesh;
}