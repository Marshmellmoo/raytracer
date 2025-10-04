#include "cube.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Cube::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint, glm::vec3& normal) const {

    float frontFace = 0.5f;
    float backFace = -0.5f;

    std::vector<float> tVals = std::vector<float>();

    // X Face Intersections
    if (ray.direction.x != 0) {

    float t1 = (frontFace - ray.origin.x) / ray.direction.x;
    float t2 = (backFace - ray.origin.x) / ray.direction.x;

    glm::vec3 p1 = ray.origin + t1 * ray.direction;
    glm::vec3 p2 = ray.origin + t2 * ray.direction;

    if ((p1.y < 0.5 && p1.y > -0.5) && (p1.z < 0.5 && p1.z > -0.5) && t1 > 0) tVals.push_back(t1);
    if ((p2.y < 0.5 && p2.y > -0.5) && (p2.z < 0.5 && p2.z > -0.5) && t2 > 0) tVals.push_back(t2);

    }

    // Y Face Intersections
    if (ray.direction.y != 0) {

    float t3 = (frontFace - ray.origin.y) / ray.direction.y;
    float t4 = (backFace - ray.origin.y) / ray.direction.y;

    glm::vec3 p3 = ray.origin + t3 * ray.direction;
    glm::vec3 p4 = ray.origin + t4 * ray.direction;

    if ((p3.x < 0.5 && p3.x > -0.5) && (p3.z < 0.5 && p3.z > -0.5) && t3 > 0) tVals.push_back(t3);
    if ((p4.x < 0.5 && p4.x > -0.5) && (p4.z < 0.5 && p4.z > -0.5) && t4 > 0) tVals.push_back(t4);

    }
    // Z Face Intersections
    if (ray.direction.z != 0) {

    float t5 = (frontFace - ray.origin.z) / ray.direction.z;
    float t6 = (backFace - ray.origin.z) / ray.direction.z;

    glm::vec3 p5 = ray.origin + t5 * ray.direction;
    glm::vec3 p6 = ray.origin + t6 * ray.direction;

    if ((p5.x < 0.5 && p5.x > -0.5) && (p5.y < 0.5 && p5.y > -0.5) && t5 > 0) tVals.push_back(t5);
    if ((p6.x < 0.5 && p6.x > -0.5) && (p6.y < 0.5 && p6.y > -0.5) && t6 > 0) tVals.push_back(t6);

    }

    if (!tVals.empty()) {

        t = *std::min_element(tVals.begin(), tVals.end());
        hitPoint = ray.origin + t * ray.direction;

        const float epsilon = 0.0001f;
        if (abs(hitPoint.x - 0.5f) < epsilon) { normal = glm::vec3(1, 0, 0); }
        else if (abs(hitPoint.x + 0.5f) < epsilon) { normal = glm::vec3(-1, 0, 0); }
        else if (abs(hitPoint.y - 0.5f) < epsilon) { normal = glm::vec3(0, 1, 0); }
        else if (abs(hitPoint.y + 0.5f) < epsilon) { normal = glm::vec3(0, -1, 0); }
        else if (abs(hitPoint.z - 0.5f) < epsilon) { normal = glm::vec3(0, 0, 1); }
        else { normal = glm::vec3(0, 0, -1); }

        return true;

    }

    return false;

}
