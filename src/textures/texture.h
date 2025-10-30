#pragma once
#include <glm/glm.hpp>
#include "utils/imagereader.h"
#include "utils/sceneparser.h"

class Texture {

public:

    Image texture;
    std::vector<Image*> m_levels;

    void generateMaps();
    glm::vec4 sampleBilinear(float s, float t, float level);
    glm::vec4 sampleTrilinear(float s, float t, float fractionalLevel);

private:
    Image downsample(double& scaleX, double& scaleY, int& width, int& height);
    glm::vec4 tent(int& k, double& a, std::vector<RGBA> f);

};
