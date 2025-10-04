#include "cone.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Cone::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint, glm::vec3& normal) const {

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
    float t1 = (-b + glm::sqrt(d)) / (2 * a);
    float t2 = (-b - glm::sqrt(d)) / (2 * a);

    glm::vec3 p1 = ray.origin + t1 * ray.direction;
    glm::vec3 p2 = ray.origin + t2 * ray.direction;

    if ((p1.y < 0.5 && p1.y > -0.5) && t1 > 0) tVals.push_back(t1);
    if ((p2.y < 0.5 && p2.y > -0.5) && t2 > 0) tVals.push_back(t2);

    // Base Intersection
    float t3 = (base - ray.origin.y) / ray.direction.y;
    glm::vec3 p3 = ray.origin + t3 * ray.direction;

    if ((((p3.x * p3.x) + (p3.z * p3.z)) < 0.25) && t3 > 0) tVals.push_back(t3);

    if (!tVals.empty()) {

        t = *std::min_element(tVals.begin(), tVals.end());
        hitPoint = ray.origin + t * ray.direction;
        if (hitPoint.y == -0.5) {
            normal = glm::normalize(glm::vec3(0, -1, 0));
        } else {
            normal = glm::normalize(glm::vec3(hitPoint.x, (0.5 - hitPoint.y) / 4, hitPoint.z));
        }

        return true;

    }

    return false;

}
