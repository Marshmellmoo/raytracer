#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "shape.h"

class Cone : public Shape {

public:

    bool rayIntersect(
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint) const override;

    glm::vec3 computeNormal(glm::vec3& hitPoint ) const override;
    glm::vec2 computeUV( glm::vec3& hitPoint ) const override;

};
