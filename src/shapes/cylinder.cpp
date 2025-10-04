#include "cylinder.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Cylinder::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint, glm::vec3& normal) const {

    float topCap = 0.5;
    float bottomCap = -0.5;

    float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
    float b = 2.0f * ray.origin.x * ray.direction.x + 2.0f * ray.origin.z * ray.direction.z;
    float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - 0.25f;
    float d = discriminant(a, b, c);

    std::vector<float> tVals = std::vector<float>();

    if (!(d < 0)) {

        // Cylinder Face Intersection
        float t1 = (-b + glm::sqrt(d)) / (2 * a);
        float t2 = (-b - glm::sqrt(d)) / (2 * a);

        glm::vec3 p1 = ray.origin + t1 * ray.direction;
        glm::vec3 p2 = ray.origin + t2 * ray.direction;

        if ((p1.y < 0.5 && p1.y > -0.5) && t1 > 0) tVals.push_back(t1);
        if ((p2.y < 0.5 && p2.y > -0.5) && t2 > 0) tVals.push_back(t2);

    }

    // Cap Intersection
    if (ray.direction.y != 0) {

    float t3 = (topCap - ray.origin.y) / ray.direction.y;
    float t4 = (bottomCap - ray.origin.y) / ray.direction.y;

    glm::vec3 p3 = ray.origin + t3 * ray.direction;
    glm::vec3 p4 = ray.origin + t4 * ray.direction;

    if ((((p3.x * p3.x) + (p3.z * p3.z)) < 0.25f) && t3 > 0) tVals.push_back(t3);
    if ((((p4.x * p4.x) + (p4.z * p4.z)) < 0.25f) && t4 > 0) tVals.push_back(t4);

    }

    if (!tVals.empty()) {

        t = *std::min_element(tVals.begin(), tVals.end());
        hitPoint = ray.origin + t * ray.direction;

        // Normal Calculation
        const float epsilon = 0.0001f;
        if (abs(hitPoint.y - 0.5f) < epsilon) {

            normal = glm::vec3(0, 1, 0);

        } else if (abs(hitPoint.y + 0.5f) < epsilon) {

            normal = glm::vec3(0, -1, 0);

        } else {

            normal = glm::normalize(glm::vec3(hitPoint.x, 0, hitPoint.z));

        }

        return true;

    }

    return false;


}
