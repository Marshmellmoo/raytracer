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

    return glm::vec2(u, v);

}

std::tuple<glm::vec4> Sphere::computeDifferentials(glm::vec3& hitPoint) const {

    float duDX, duDY, duDZ;
    float dvDX, dvDY, dvDZ;

    duDX = (1 / (2 * M_PI)) *
           (hitPoint.z / (hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z));

    duDY = 0;

    duDZ = (-1 / (2 * M_PI)) *
           (hitPoint.z / (hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z));

    dvDX = 0;
    dvDY = 1 / (M_PI * glm::sqrt(m_radius * m_radius - hitPoint.y * hitPoint.y));
    dvDZ = 0;

    glm::vec3 du = glm::vec3(duDX, duDY, duDZ);
    glm::vec3 dv = glm::vec3(dvDX, dvDY, dvDZ);

    std::tuple<glm::vec3> result = std::tuple<glm::vec3>(2)[du, dv];
    return std::tuple<glm::vec4>(2);

}
