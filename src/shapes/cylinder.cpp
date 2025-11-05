#include "cylinder.h"
#include <glm/glm.hpp>
#include <algorithm>

bool Cylinder::rayIntersect(const Ray& ray, float& t, glm::vec3& hitPoint) const {

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

        return true;

    }

    return false;


}

glm::vec3 Cylinder::computeNormal(glm::vec3& hitPoint) const {

    glm::vec3 normal;
    const float epsilon = 0.0001f;

    if (abs(hitPoint.y - 0.5f) < epsilon) {

        normal = glm::vec3(0, 1, 0);

    } else if (abs(hitPoint.y + 0.5f) < epsilon) {

        normal = glm::vec3(0, -1, 0);

    } else {

        normal = glm::normalize(glm::vec3(hitPoint.x, 0, hitPoint.z));

    }

    return normal;

}

glm::vec2 Cylinder::computeUV( glm::vec3& hitPoint ) const {

    const float epsilon = 0.0001f;
    float u, v;

    if (abs(hitPoint.y - 0.5f) < epsilon) {

        u = hitPoint.x + 0.5f;
        v = -hitPoint.z + 0.5f;

    } else if (abs(hitPoint.y + 0.5f) < epsilon) {

        u = hitPoint.x + 0.5f;
        v = hitPoint.z + 0.5f;

    } else {

        float theta = atan2(hitPoint.z, hitPoint.x);
        if (theta < 0) u = (-theta) / (2.0f * M_PI);
        else u = 1.0f - (theta / (2.0f * M_PI));

        v = hitPoint.y + 0.5f;

    }

    return glm::vec2(u, v);
}

std::tuple<glm::vec3, glm::vec3> Cylinder::computeDifferentials(glm::vec3& hitPoint) const {

    const float epsilon = 0.0001f;

    glm::vec3 du_dp, dv_dp;

    float du_dx, du_dy, du_dz;
    float dv_dx, dv_dy, dv_dz;

    if (abs(hitPoint.y - 0.5f) < epsilon) {

        du_dp = glm::vec3(1, 0, 0);
        dv_dp = glm::vec3(0, 0, -1);

    } else if (abs(hitPoint.y + 0.5f) < epsilon) {

        du_dp = glm::vec3(1, 0, 0);
        dv_dp = glm::vec3(0, 0, 1);

    } else {

        du_dx = (-2 * hitPoint.z) / M_PI;
        du_dy = 0;
        du_dz = (2 * hitPoint.x) / M_PI;

        du_dp = glm::vec3(du_dx, du_dy, du_dz);

        dv_dx = 0;
        dv_dy = 1;
        dv_dz = 0;

        dv_dp = glm::vec3(dv_dx, dv_dy, dv_dz);

    }

    return std::make_tuple(du_dp, dv_dp);

}
