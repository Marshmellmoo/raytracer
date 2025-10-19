#include "cone.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Cone::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint) const {

    float base = -0.5;

    float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z
              - (0.25f * ray.direction.y * ray.direction.y);
    float b = 2.0f * ray.origin.x * ray.direction.x + 2.0f * ray.origin.z * ray.direction.z
              - 0.5f * ray.origin.y * ray.direction.y + 0.25f * ray.direction.y;
    float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z
              - (0.25f * ray.origin.y * ray.origin.y) + (0.25f * ray.origin.y) - (1.0f/16.0f);
    float d = discriminant(a, b, c);

    std::vector<float> tVals = std::vector<float>();

    // Conical Top Intersection
    if (!(d < 0)) {

        float t1 = (-b + glm::sqrt(d)) / (2 * a);
        float t2 = (-b - glm::sqrt(d)) / (2 * a);

        glm::vec3 p1 = ray.origin + t1 * ray.direction;
        glm::vec3 p2 = ray.origin + t2 * ray.direction;

        if ((p1.y < 0.5 && p1.y > -0.5) && t1 > 0) tVals.push_back(t1);
        if ((p2.y < 0.5 && p2.y > -0.5) && t2 > 0) tVals.push_back(t2);

    }

    // Base Intersection
    if (ray.direction.y != 0){

        float t3 = (base - ray.origin.y) / ray.direction.y;
        glm::vec3 p3 = ray.origin + t3 * ray.direction;

        if ((((p3.x * p3.x) + (p3.z * p3.z)) < 0.25) && t3 > 0) tVals.push_back(t3);

    }

    if (!tVals.empty()) {

        t = *std::min_element(tVals.begin(), tVals.end());
        hitPoint = ray.origin + t * ray.direction;

        return true;

    }

    return false;

}

glm::vec3 Cone::computeNormal(glm::vec3& hitPoint) const {

    glm::vec3 normal;

    const float epsilon = 0.0001f;
    if (abs(hitPoint.y + 0.5f) < epsilon) {
        normal = glm::vec3(0, -1, 0);
    } else {
        normal = glm::normalize(glm::vec3(hitPoint.x, (0.5f - hitPoint.y) / 4.0f, hitPoint.z));
    }

    return normal;

}

glm::vec2 Cone::computeUV(glm::vec3& hitPoint) const {


    const float epsilon = 0.0001f;
    float u, v;

    if (abs(hitPoint.y + 0.5f) < epsilon) {

        u = hitPoint.x + 0.5;
        v = hitPoint.z + 0.5;

    } else {

        float theta = atan2(-hitPoint.z, hitPoint.x);
        u = (theta + M_PI) / (2 * M_PI) + 0.5;
        v = hitPoint.y + 0.5;

    }

    return glm::vec2(u, v);

}
