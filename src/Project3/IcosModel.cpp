#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <filesystem>

#include "renderer.h"

#include "IcosModel.h"

IcosModel::IcosModel(Material* material, glm::mat4 m)
{
    Renderer::name = "Icosahedron";
    Icosahedron myIcosahedron;
    // set up vertex data (and buffer(s)) and configure vertex attributes
    modelMatrix = m;

    myMaterial = material;

    glGenVertexArrays(1, &VAO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glGenBuffers(numVBOs = 2, VBO);
    glGenBuffers(1, &EBO);

    int numIndices = myIcosahedron.getIndices().size();

    indexCount = numIndices;

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, myIcosahedron.getVerts().size() * 4, myIcosahedron.getVerts().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myIcosahedron.getIndices().size() * 4, myIcosahedron.getIndices().data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal vector attribute  :  REUSE THE VERTEX COORDINATES !!!
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);

    setupColorAttrib();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(0);
};


Icosahedron::Icosahedron() {
    init();
}

float Icosahedron::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void Icosahedron::init() {
    numVertices = 12 * 3;
    numIndices = 20 * 3;
    float x = 0.525731112119133606f;
    float z = 0.850650808352039932f;

    // calculate triangle verts
    verts = {
       -x, 0.0, z, 0.0f, 0.0f,
       x, 0.0, z, 0.5f, 1.0f,
       -x, 0.0, -z, 1.0f, 1.0f,

       x, 0.0, -z, 1.0f, 1.0f,
       0.0, z, x, 1.0f, 0.0f,
       0.0, z, -x, 0.5f, 1.0f,

       0.0, -z, x, 0.5f, 1.0f,
       0.0, -z, -x, 0.0f, 0.0f,
       z, x, 0.0, 0.0f, 0.0f,

       -z, x, 0.0, 0.0f, 0.0f,
       z, -x, 0.0, 0.5f, 1.0f ,
       -z, -x, 0.0, 0.5f, 1.0f
    };

    //calculating the index array
	indices = {
        0, 4, 1,
        0, 9, 4,
        9, 5, 4,
	    4, 5, 8,
	    4, 8, 1,
        8, 10, 1,
	    8, 3, 10,
	    5, 3, 8,
	    5, 2, 3,
	    2, 7, 3,
        7, 10, 3,
	    7, 6, 10,
	    7, 11, 6,
	    11, 0, 6,
	    0, 1, 6,
        6, 1, 10,
	    9, 0, 11,
	    9, 11, 2,
	    9, 2, 5,
	    7, 2, 11
	};
}

int Icosahedron::getNumVertices() { return numVertices; }
int Icosahedron::getNumIndices() { return numIndices; }
std::vector<int> Icosahedron::getIndices() { return indices; }
std::vector<float> Icosahedron::getVerts() { return verts; }

