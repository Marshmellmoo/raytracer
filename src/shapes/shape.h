#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "utils/imagereader.h"
#include "utils/sceneparser.h"
#include "utils/imagereader.h"

class Shape {

public:

    virtual ~Shape() {}

    virtual bool rayIntersect(
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint) const = 0;

    virtual glm::vec3 computeNormal(glm::vec3& hitPoint ) const = 0;
    virtual glm::vec2 computeUV( glm::vec3& hitPoint ) const = 0;

    RenderShapeData shapeInfo;

    glm::mat4 inverseCTM;
    Image* textureMap;

protected:

    float discriminant(float a, float b, float c) const {
        return b * b - 4 * a * c;
    }

};
