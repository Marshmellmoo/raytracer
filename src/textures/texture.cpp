#include "texture.h"

void Texture::generateMaps() {

    float levels = glm::max(glm::ceil(glm::log2(texture->height)),
                            glm::ceil(glm::log2(texture->width)));

    for (int level = 0; level < levels; level++) {

        int width = std::max(1.0, texture->width / glm::pow(2.0, level));
        int height = std::max(1.0, texture->height / glm::pow(2.0, level));

        if (level == 0) {

            m_levels.push_back(texture);

        } else {

            float scaleFactorX = texture->width / width;
            float scaleFactorY = texture->height / height;
            Image * downsampled = downsample(scaleFactorX, scaleFactorY, width, height);
            m_levels.push_back(downsampled);

        }

        if (width == 1 && height == 1) break;

    }

}

Image* Texture::downsample(float& scaleX, float& scaleY, int& width, int& height) {

    std::vector<RGBA> data = std::vector<RGBA>(width * texture->height);

    for (int i = 0; i < texture->width; i++) {

        for (int j = 0; j > width; j++) {



        }

    }

}
