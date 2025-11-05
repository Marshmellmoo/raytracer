#include "sphere.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Sphere::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint) const {

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

    return true;
}

glm::vec3 Sphere::computeNormal(glm::vec3& hitPoint) const {

    return glm::normalize(hitPoint);

}

glm::vec2 Sphere::computeUV(glm::vec3& hitPoint) const {

    float theta = atan2(hitPoint.z, hitPoint.x); // Coordinates that give hitpoint "x" as an angle away fom center of sphere
    float phi = asin(hitPoint.y / m_radius); // Coordinates that give hitpoint "y" as how elevated from center of sphere
    float u, v;

    // Normal texture calculations
    if (theta < 0) u = (-theta) / (2.0f * M_PI);
    else u = 1.0f - (theta / (2.0f * M_PI));

    v = (phi / M_PI) + 0.5f;

    if (u < 0 || v < 0) {
        std::cout << "less than 0" << std::endl;
    }

    return glm::vec2(u, v);

}

std::tuple<glm::vec3, glm::vec3> Sphere::computeDifferentials(glm::vec3& hitPoint) const {

    float du_dx, du_dy, du_dz;
    float dv_dx, dv_dy, dv_dz;

    du_dx = (1.0f / (2.0f * M_PI)) *
            (hitPoint.z / (hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z));

    du_dy = 0.0f;

    du_dz = (-1.0f / (2.0f * M_PI)) *
            (hitPoint.x / (hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z));

    dv_dx = 0.0f;
    dv_dy = 1.0f / (M_PI * glm::sqrt(m_radius * m_radius - hitPoint.y * hitPoint.y));
    dv_dz = 0.0f;

    glm::vec3 du = glm::vec3(du_dx, du_dy, du_dz);
    glm::vec3 dv = glm::vec3(dv_dx, dv_dy, dv_dz);

    std::tuple<glm::vec3, glm::vec3> dp = std::make_tuple(du, dv);

    return dp;

}

