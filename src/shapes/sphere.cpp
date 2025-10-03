#include "sphere.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Sphere::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint, glm::vec3& normal) const {

    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(ray.origin, ray.direction);
    float c = glm::dot(ray.origin, ray.origin) - (m_radius * m_radius);

    float d = discriminant(a, b, c);
    if (d < 0) return false;

    float t1 = (-b + glm::sqrt(d)) / (2 * a);
    float t2 = (-b - glm::sqrt(d)) / (2 * a);

    if (t1 < 0 && t2 < 0) return false;

    t = (t1 > 0 && t2 > 0) ? std::min(t1, t2) : std::max(t1, t2);
    hitPoint = ray.origin + t * ray.direction;
    normal = glm::normalize(hitPoint);

    return true;
}
