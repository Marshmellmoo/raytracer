#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"
#include <functional>

// A class representing a scene to be ray-traced

// Feel free to make your own design choices for RayTraceScene, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

// PRIMITIVE_CUBE,
//     PRIMITIVE_CONE,
//     PRIMITIVE_CYLINDER,
//     PRIMITIVE_SPHERE,
//     PRIMITIVE_MESH

struct Shape {

    virtual bool rayIntersect(Ray& ray, float& t, glm::vec3& t_point, glm::vec3& normal) const = 0;
    float discriminant(float a, float b, float c) const {

        auto d = b * b - (4 * a * c) / (2 * a);
        return d;

    }
};

struct Cube : Shape {

    float frontFace = 0.5f;
    float backFace = -0.5f;

    bool rayIntersect(Ray& ray, float& t, glm::vec3& t_point, glm::vec3& normal) const override {

        std::vector<float> tVals = std::vector<float>();

        // X Face Intersections
        float t1 = (frontFace - ray.origin.x) / ray.direction.x;
        float t2 = (backFace - ray.origin.x) / ray.direction.x;

        glm::vec3 p1 = ray.origin + t1 * ray.direction;
        glm::vec3 p2 = ray.origin + t2 * ray.direction;

        if ((p1.y < 0.5 && p1.y > -0.5) && (p1.z < 0.5 && p1.z > -0.5) && t1 > 0) tVals.push_back(t1);
        if ((p2.y < 0.5 && p2.y > -0.5) && (p2.z < 0.5 && p2.z > -0.5) && t2 > 0) tVals.push_back(t2);

        // Y Face Intersections
        float t3 = (frontFace - ray.origin.y) / ray.direction.y;
        float t4 = (backFace - ray.origin.y) / ray.direction.y;

        glm::vec3 p3 = ray.origin + t3 * ray.direction;
        glm::vec3 p4 = ray.origin + t4 * ray.direction;

        if ((p3.x < 0.5 && p3.x > -0.5) && (p3.z < 0.5 && p3.z > -0.5) && t3 > 0) tVals.push_back(t3);
        if ((p4.x < 0.5 && p4.x > -0.5) && (p4.z < 0.5 && p4.z > -0.5) && t4 > 0) tVals.push_back(t4);

        // Z Face Intersections
        float t5 = (frontFace - ray.origin.z) / ray.direction.z;
        float t6 = (backFace - ray.origin.z) / ray.direction.z;

        glm::vec3 p5 = ray.origin + t5 * ray.direction;
        glm::vec3 p6 = ray.origin + t6 * ray.direction;

        if ((p5.x < 0.5 && p5.x > -0.5) && (p5.y < 0.5 && p5.y > -0.5) && t5 > 0) tVals.push_back(t5);
        if ((p6.x < 0.5 && p6.x > -0.5) && (p6.y < 0.5 && p6.y > -0.5) && t6 > 0) tVals.push_back(t6);

        if (!tVals.empty()) {

            t = *std::min_element(tVals.begin(), tVals.end());
            t_point = ray.origin + t * ray.direction;
            normal = glm::normalize(t_point);

            return true;

        }

        return false;

    }

};

struct Sphere : Shape {

    float radius;
    bool rayIntersect(Ray& ray, float& t, glm::vec3& t_point, glm::vec3& normal) const override{

        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(ray.origin, ray.direction);
        float c = glm::dot(ray.origin, ray.origin) - (radius * radius);

        float d = discriminant(a, b, c);

        if (d < 0) {
            return false;
        }

        float t1 = (-b + glm::sqrt(d)) / (2 * a);
        float t2 = (-b - glm::sqrt(d)) / (2 * a);

        if (t1 < 0 && t2 < 0) return false;

        t = (t1 > 0 && t2 > 0) ? std::min(t1, t2) : std::max(t1, t2);
        t_point = ray.origin + t * ray.direction;
        normal = glm::normalize(t_point);

        return true;

    }

};

struct Cylinder : Shape {

    float topCap = 0.5;
    float bottomCap = -0.5;

    bool rayIntersect(Ray& ray, float& t, glm::vec3& t_point, glm::vec3& normal) const override {

        float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
        float b = 2.0f * ray.origin.x * ray.direction.x + 2.0f * ray.origin.z * ray.direction.z;
        float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - 1;
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
        float t3 = (topCap / ray.origin.y) - ray.direction.y;
        float t4 = (bottomCap / ray.origin.y) - ray.direction.y;

        glm::vec3 p3 = ray.origin + t3 * ray.direction;
        glm::vec3 p4 = ray.origin + t4 * ray.direction;

        if ((((p3.x * p3.x) + (p3.z * p3.z)) < 1) && t3 > 0) tVals.push_back(t3);
        if ((((p4.x * p4.x) + (p4.z * p4.z)) < 1) && t4 > 0) tVals.push_back(t4);

        if (!tVals.empty()) {

            t = *std::min_element(tVals.begin(), tVals.end());
            t_point = ray.origin + t * ray.direction;
            normal = glm::normalize(t_point);

            return true;

        }

        return false;

    }

};

struct Cone : Shape {

    float base = -0.5;

    bool rayIntersect(Ray& ray, float& t, glm::vec3& t_point, glm::vec3& normal) const override {

        float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z
                  - ((1/4) * ray.direction.y * ray.direction.y);
        float b = 2.0f * ray.origin.x * ray.direction.x + 2.0f * ray.origin.z * ray.direction.z
                  - ((1/2) * ray.origin.y * ray.direction.y + (1/4) * ray.direction.y);
        float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z
                  - ((1/4) * ray.origin.y * ray.origin.y) + ((1/4) * ray.origin.y) - (1/16);
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
        float t3 = (base / ray.origin.y) - ray.direction.y;
        glm::vec3 p3 = ray.origin + t3 * ray.direction;

        if ((((p3.x * p3.x) + (p3.z * p3.z)) < (base * base)) && t3 > 0) tVals.push_back(t3);

        if (!tVals.empty()) {

            t = *std::min_element(tVals.begin(), tVals.end());
            t_point = ray.origin + t * ray.direction;
            normal = glm::normalize(t_point);

            return true;

        }

        return false;

    }

};

class RayTraceScene
{

private:


public:
    RayTraceScene(int width, int height, const RenderData &metaData);

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of the shared pointer to the camera instance of the scene
    const Camera& getCamera() const;
};
