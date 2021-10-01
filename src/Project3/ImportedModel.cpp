#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <filesystem>

#include "renderer.h"
#include "ImportedModel.h"

using namespace std;

ImportedModel::ImportedModel() {}

ImportedModel::ImportedModel(const char* filePath) {

}

int ImportedModel::getNumVertices() { return numVertices; }
std::vector<glm::vec3> ImportedModel::getVertices() { return vertices; }
std::vector<glm::vec2> ImportedModel::getTextureCoords() { return texCoords; }
std::vector<glm::vec3> ImportedModel::getNormals() { return normalVecs; }

// ---------------------------------------------------------------

ModelImporter::ModelImporter() {}

ObjModel::ObjModel(const char* filePath, Material* material, glm::mat4 m)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    modelMatrix = m;

    myMaterial = material;

    glGenVertexArrays(1, &VAO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glGenBuffers(numVBOs = 2, VBO);
    glGenBuffers(1, &EBO);

    ModelImporter modelImporter = ModelImporter();
    modelImporter.parseOBJ(filePath);

    std::vector<float> verts = modelImporter.getVertices();
    std::vector<float> tcs = modelImporter.getTextureCoordinates();
    std::vector<float> normals = modelImporter.getNormals();

    std::vector<float> vbovalues;
    std::vector<int> indices;

    for (int i = 0; i < modelImporter.getNumVertices(); i++) {
        vbovalues.push_back(verts[i * 3]);
        vbovalues.push_back(verts[i * 3 + 1]);
        vbovalues.push_back(verts[i * 3 + 2]);

        vbovalues.push_back(normals[i * 3]);
        vbovalues.push_back(normals[i * 3 + 1]);
        vbovalues.push_back(normals[i * 3 + 2]);

        vbovalues.push_back(tcs[i * 2]);
        vbovalues.push_back(tcs[i * 2 + 1]);

        indices.push_back(i);
    }

    indexCount = indices.size();

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vbovalues.size() * 4, &vbovalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, &indices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal vector attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // skip color for now since its in another VBO
    // 
    // texture coord attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);

    setupColorAttrib();

    glBindVertexArray(0);
};

void ModelImporter::parseOBJ(const char* filePath) {
    float x, y, z;
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        if (line.compare(0, 2, "v ") == 0) {
            stringstream ss(line.erase(0, 1));
            ss >> x; ss >> y; ss >> z;
            vertVals.push_back(x);
            vertVals.push_back(y);
            vertVals.push_back(z);
        }
        if (line.compare(0, 2, "vt") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y;
            stVals.push_back(x);
            stVals.push_back(y);
        }
        if (line.compare(0, 2, "vn") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y; ss >> z;
            normVals.push_back(x);
            normVals.push_back(y);
            normVals.push_back(z);
        }
        if (line.compare(0, 2, "f ") == 0) {
            string oneCorner, v, t, n;
            stringstream ss(line.erase(0, 2));
            int vi[4],ti[4],ni[4];
            for (int i = 0; i < 4; i++) {
                //int vi, ti, ni;
                vi[i] = ti[i] = ni[i] = 0;
                while (ss.peek() == ' ') // skip spaces
                    ss.get();

                getline(ss, oneCorner, ' ');

                if (!ss)
                    break;
                if (sscanf(oneCorner.c_str(), "%i/%i/%i", &vi[i], &ti[i], &ni[i]) != 3) {
                    sscanf(oneCorner.c_str(), "%i//%i", &vi[i], &ni[i]);
                }

                if (ti[i] < 1) {
                    if (stVals.size() / 2 >= vi[i])
                        ti[i] = vi[i];
                }

                if (ni[i] < 1) {
                    if (normVals.size() / 3 >= vi[i])
                        ni[i] = vi[i];
                }

                if (i > 2){
                    for (int j = 1; j < 3; j++){
                        
                        int vertRef = (vi[j] - 1) * 3;
                        int tcRef = (ti[j] - 1) * 2;
                        int normRef = (ni[j] - 1) * 3;
                        
                        if (vertRef > -1) {
                            triangleVerts.push_back(vertVals[vertRef]);
                            triangleVerts.push_back(vertVals[vertRef + 1]);
                            triangleVerts.push_back(vertVals[vertRef + 2]);
                        }
                        else {
                            triangleVerts.push_back(0.0f);
                            triangleVerts.push_back(0.0f);
                            triangleVerts.push_back(0.0f);
                        }

                        if (tcRef > -1) {
                            textureCoords.push_back(stVals[tcRef]);
                            textureCoords.push_back(stVals[tcRef + 1]);
                        }
                        else {
                            textureCoords.push_back(0.0f);
                            textureCoords.push_back(0.0f);
                        }

                        if (normRef > -1) {
                            normals.push_back(normVals[normRef]);
                            normals.push_back(normVals[normRef + 1]);
                            normals.push_back(normVals[normRef + 2]);
                        }
                        else {
                            normals.push_back(0.0f);
                            normals.push_back(0.0f);
                            normals.push_back(0.0f);
                        }
                    }
                }

                int vertRef = (vi[i] - 1) * 3;
                int tcRef = (ti[i] - 1) * 2;
                int normRef = (ni[i] - 1) * 3;
                
                if (vertRef > -1) {
                    triangleVerts.push_back(vertVals[vertRef]);
                    triangleVerts.push_back(vertVals[vertRef + 1]);
                    triangleVerts.push_back(vertVals[vertRef + 2]);
                }
                else {
                    triangleVerts.push_back(0.0f);
                    triangleVerts.push_back(0.0f);
                    triangleVerts.push_back(0.0f);
                }

                if (tcRef > -1) {
                    textureCoords.push_back(stVals[tcRef]);
                    textureCoords.push_back(stVals[tcRef + 1]);
                }
                else {
                    textureCoords.push_back(0.0f);
                    textureCoords.push_back(0.0f);
                }

                if (normRef > -1) {
                    normals.push_back(normVals[normRef]);
                    normals.push_back(normVals[normRef + 1]);
                    normals.push_back(normVals[normRef + 2]);
                }
                else {
                    normals.push_back(0.0f);
                    normals.push_back(0.0f);
                    normals.push_back(0.0f);
                }
            }
        }
    }
}
int ModelImporter::getNumVertices() { return (triangleVerts.size() / 3); }
std::vector<float> ModelImporter::getVertices() { return triangleVerts; }
std::vector<float> ModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float> ModelImporter::getNormals() { return normals; }
