#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "utils/sceneparser.h"

class Shape {

public:

    virtual ~Shape() {}

    virtual bool rayIntersect(
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint,
        glm::vec3& normal ) const = 0;

    RenderShapeData shapeInfo;

protected:

    float discriminant(float a, float b, float c) const {
        return b * b - 4 * a * c;
    }

};
