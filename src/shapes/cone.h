#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "shape.h"

class Cone : public Shape {

public:

    bool rayIntersect(
        const Ray& ray,
        float& t,
        glm::vec3& hitPoint,
        glm::vec3& normal) const override;

};
