#pragma once

#include <vector>
#include "Material.h"
#include "Renderer.h"

class IcosModel : public Renderer {
public:
    IcosModel(Material*, glm::mat4 m);
};

class Icosahedron
{
private:
    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<float> verts;
    void init();
    float toRadians(float degrees);

public:
    Icosahedron();
    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<float> getVerts();
};