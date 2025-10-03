#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "shape.h"

class Sphere : public Shape {

public:

    Sphere(float radius = 0.5f) : m_radius(radius) {}
    bool rayIntersect(
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint,
        glm::vec3& normal
        ) const override;

private:
    float m_radius;

};
