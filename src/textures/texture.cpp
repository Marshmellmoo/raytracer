#include "texture.h"

Texture::Texture() : texture(nullptr), blend(0.0f) { info = SceneFileMap(); }

Texture::Texture(Image* text,
                 SceneFileMap i,
                 float b) : texture(text),
                 info(i),
                 blend(b) {}

//                                                  === HELPERS ===
inline int pointToIndex(int i, int j, int width) {

    return j * width + i;

}
inline RGBA toRGBA(const glm::vec4 &illumination) {

    uint8_t r = (uint8_t)(255.0f * glm::min(glm::max(illumination[0], 0.0f), 1.0f));
    uint8_t g = (uint8_t)(255.0f * glm::min(glm::max(illumination[1], 0.0f), 1.0f));
    uint8_t b = (uint8_t)(255.0f * glm::min(glm::max(illumination[2], 0.0f), 1.0f));

    return RGBA{r, g, b, 255};

}
glm::vec4 toFloat(const RGBA &illumination) {

    float r = (float)(glm::min(glm::max(illumination.r / 255.0f, 0.0f), 1.0f));
    float g = (float)(glm::min(glm::max(illumination.g / 255.0f, 0.0f), 1.0f));
    float b = (float)(glm::min(glm::max(illumination.b / 255.0f, 0.0f), 1.0f));

    return glm::vec4(r, g, b, 1.0f);

}





//                                                  === TEXTURE HANDLING ===
void Texture::generateMaps() {

    m_levels = std::vector<Image*>();

    float levels = glm::max(glm::ceil(glm::log2((float)texture->height)),
                            glm::ceil(glm::log2((float)texture->width)));

    for (int level = 0; level <= levels; level++) {


        int width = std::max(1.0, texture->width / glm::pow(2.0, level));
        int height = std::max(1.0, texture->height / glm::pow(2.0, level));


        if (level == 0) {

            m_levels.push_back(texture);

        } else {

            float scaleFactorX = (float)width / texture->width;
            float scaleFactorY = (float)height / texture->height;
            Image* downsampledImage = downsample(scaleFactorX, scaleFactorY, width, height);
            m_levels.push_back(downsampledImage);

        }

        if (width == 1 && height == 1) break;

    }

}

Image* Texture::downsample(float& scaleX, float& scaleY, int& width, int& height) {

    std::vector<RGBA> horizontal = std::vector<RGBA>(width * texture->height);
    std::vector<RGBA> vertical = std::vector<RGBA>(width * height);
    std::vector<RGBA> textureData(texture->data, texture->data + texture->width * texture->height);

    // Horizontal Pass --
    for (int j = 0; j < texture->height; j++) {
        for (int i = 0; i < width; i++) {

            int index = pointToIndex(i, j, width);
            horizontal[index] = toRGBA(tent(i, scaleX, textureData, j, i, texture->width, texture->height, true));

        }
    }

    RGBA* data = new RGBA[width * height];

    // Vertical Pass --
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            int index = pointToIndex(i, j, width);
            RGBA color = toRGBA(tent(j, scaleY, horizontal, j, i, width, texture->height, false));
            vertical[index] = color;
            data[index] = color;

        }
    }


    // // DEBUG
    // QImage imgH(width, texture->height, QImage::Format_RGBA8888);
    // for (int row = 0; row < texture->height; row++) {  // Original height
    //     for (int col = 0; col < width; col++) {        // Scaled width
    //         int index = col + row * width;  // Direct calculation, clear order
    //         RGBA pixel = horizontal[index];
    //         imgH.setPixelColor(col, row, QColor(pixel.r, pixel.g, pixel.b, pixel.a));
    //     }
    // }

    // std::string filenameH = "debug_hpass_" + std::to_string(scaleX) + ".png";
    // imgH.save(QString::fromStdString(filenameH));

    // // Save final vertical pass result
    // QImage img(width, height, QImage::Format_RGBA8888);
    // for (int row = 0; row < height; row++) {  // Final height
    //     for (int col = 0; col < width; col++) { // Final width
    //         int index = col + row * width;
    //         RGBA pixel = vertical[index];
    //         img.setPixelColor(col, row, QColor(pixel.r, pixel.g, pixel.b, pixel.a));
    //     }
    // }

    // std::string filename = "debug_final_" + std::to_string(scaleX) + "x" + std::to_string(scaleY) + ".png";
    // img.save(QString::fromStdString(filename));


    Image* result = new Image{data, width, height};
    return result;

}

// Takes vec4 color values and returns the blended version.
glm::vec4 lerp(glm::vec4 a, glm::vec4 b, float weight) {

    return (1 - weight) * a + weight * b;

}

float filter(float x, float radius) {

    if (x < -radius || x > radius) return 0;
    else return (1 - fabs(x) / radius);

}


glm::vec4 Texture::tent(int& k, float& a, std::vector<RGBA>& f, int& row, int& col, int fWidth, int fHeight, bool horizontal) {

    glm::vec4 sum(0.0f);
    float weights_sum = 0.0f;

    float center = ((float)k + 0.5f) / a - 0.5f;
    float radius = (a < 1.0f) ? (1.0f / a) : 1.0f;

    int left = (int)std::floor(center - radius);
    int right = (int)std::ceil(center + radius);

    left = std::max(left, 0 - 1);
    right = std::min(right, (horizontal ? fWidth : fHeight) - 1 + 1);

    for (int s = left; s <= right; ++s) {

        int sourceIndex;

        if (horizontal) {

            int sx = ((s % fWidth) + fWidth) % fWidth;
            sourceIndex = ((row % fHeight) * fWidth) + sx;

        } else {

            int sy = ((s % fHeight) + fHeight) % fHeight;
            sourceIndex = (sy * fWidth) + (col % fWidth);

        }

        float w = filter((float)(s - center), radius);
        sum += w * toFloat(f[sourceIndex]);
        weights_sum += w;
    }

    if (weights_sum <= 0.0f) return glm::vec4(0.0f);
    return sum / weights_sum;

}

glm::vec4 Texture::sampleNearest(glm::vec2 uv) {

    int x, y;

    // Repeating texture checking, normal assignment otherwise
    x = (info.repeatU > 0) ? (int)(glm::floor(uv.x * info.repeatU * texture->width)) % texture->width :
                                 (int)(glm::floor(uv.x * texture->width));

    y = (info.repeatV > 0) ? (int)(glm::floor((1 - uv.y) * info.repeatV * texture->height)) % texture->height :
                             (int)(glm::floor((1 - uv.y) * texture->height));

    // Bounds checking
    x = glm::clamp(x, 0, texture->width - 1);
    y = glm::clamp(y, 0, texture->height - 1);

    RGBA textureColor = texture->data[pointToIndex(x, y, texture->width)];
    glm::vec4 textureColorNorm = glm::vec4((float)textureColor.r, (float)textureColor.g, (float)textureColor.b, 255.0f) / 255.0f;

    return textureColorNorm;

}

glm::vec4 Texture::sampleBilinear(glm::vec2& uv, float level, bool mipmap) {

    int b_level;
    b_level = (mipmap) ? (int)glm::clamp(glm::ceil(level), 0.0f, m_levels.size() - 1.0f) : 0;

    Image* map = m_levels[b_level];

    float x_left, x_right, y_top, y_bottom;

    x_left = (info.repeatU > 0) ? uv.x * info.repeatU * map->width - 0.5 :
                                  uv.x * map->width;

    y_top = (info.repeatV > 0) ? (1 - uv.y) * info.repeatV * map->height - 0.5 :
                                 (1 - uv.y) * map->height;

    x_right = x_left + 1;
    y_bottom = y_top + 1;


    int c_left, c_right, r_top, r_bottom;

    c_left = (int)glm::floor(x_left) % map->width;
    c_left = (c_left < 0) ? c_left + map->width : c_left;

    c_right = (int)glm::floor(x_right) % map->width;
    c_right = (c_right < 0) ? c_right + map->width : c_right;

    r_top = (int)glm::floor(y_top) % map->height;
    r_top = (r_top < 0) ? r_top + map->height : r_top;

    r_bottom = (int)glm::floor(y_bottom) % map->height;
    r_bottom = (r_bottom < 0) ? r_bottom + map->height : r_bottom;

    float a_x, a_y;

    a_x = x_left - glm::floor(x_left);
    a_y = y_top - glm::floor(y_top);


    glm::vec4 c00, c01, c10, c11;
    glm::vec4 I_top, I_bottom, I_final;

    c00 = toFloat(map->data[pointToIndex(c_left, r_top, map->width)]);
    c01 = toFloat(map->data[pointToIndex(c_right, r_top, map->width)]);
    c10 = toFloat(map->data[pointToIndex(c_left, r_bottom, map->width)]);
    c11 = toFloat(map->data[pointToIndex(c_right, r_bottom, map->width)]);

    I_top = lerp(c00, c01, a_x);
    I_bottom = lerp(c10, c11, a_x);

    I_final = lerp(I_top, I_bottom, a_y);

    return I_final;

}

glm::vec4 Texture::sampleTrilinear(glm::vec2& uv, float& fractionalLevel, bool mipmap) {

    int levelA, levelB;
    levelA = floor(fractionalLevel);
    levelB = levelA + 1;
    levelB = (int)glm::min((float)levelB, (float)m_levels.size() - 1.0f);

    glm::vec4 colorA, colorB;
    float weight;

    weight = fractionalLevel - levelA;
    colorA = sampleBilinear(uv, levelA, mipmap);
    colorB = sampleBilinear(uv, levelB, mipmap);

    return lerp(colorA, colorB, weight);

}
