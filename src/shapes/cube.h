#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "shape.h"

class Cube : public Shape {

public:

    bool rayIntersect (
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint) const override;

    glm::vec3 computeNormal(glm::vec3& hitPoint ) const override;
    glm::vec2 computeUV( glm::vec3& hitPoint ) const override;
    std::tuple<glm::vec3, glm::vec3> computeDifferentials( glm::vec3& hitPoint ) const override;
    bool pointShapeCollision(const glm::vec3 &pointObject) const override;

};
