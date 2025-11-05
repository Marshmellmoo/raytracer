#pragma once
#include <glm/glm.hpp>
#include "utils/imagereader.h"
#include "utils/sceneparser.h"

class Texture {

public:

    Image* texture;
    SceneFileMap info;
    std::vector<Image*> m_levels;
    float blend;

    Texture();
    Texture(Image* texture, SceneFileMap info, float blend);

    void generateMaps();
    glm::vec4 sampleNearest(glm::vec2 uv);
    glm::vec4 sampleBilinear(glm::vec2& uv, float& level, bool mipmap);
    glm::vec4 sampleTrilinear(glm::vec2& uv, float& fractionalLevel, bool mipmap);

    void debug(const std::string& baseFilename);


private:

    Image* downsample(float& scaleX, float& scaleY, int& width, int& height);
    glm::vec4 tent(int& k, float& a, std::vector<RGBA> f, int row, int col, int fWidth, int fHeight, bool horizontal = true);

};
