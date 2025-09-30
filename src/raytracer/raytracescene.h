#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"

// A class representing a scene to be ray-traced

// Feel free to make your own design choices for RayTraceScene, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

// PRIMITIVE_CUBE,
//     PRIMITIVE_CONE,
//     PRIMITIVE_CYLINDER,
//     PRIMITIVE_SPHERE,
//     PRIMITIVE_MESH

struct ImplicitShape {

    bool rayIntersectPoint(Ray& ray);
    float discriminant(float a, float b, float c) {

        auto d = b * b - (4 * a * c) / (2 * a);
        return d;

    }
};

struct Sphere : ImplicitShape {

    float radius;
    bool rayIntersectPoint(Ray& ray) {

        float a = glm::dot(ray.origin, ray.direction);
        float b = 2.0f * glm::dot(ray.origin, ray.direction);
        float c = glm::dot(ray.origin, ray.direction) - (radius * radius);

        float d = discriminant(a, b, c);

        if (d < 0) {
            return false;
        }

        float t1 = -b + glm::sqrt(d) / (2 * a);
        float t2 = -b - glm::sqrt(d) / (2 * a);

        float t = std::min(t1, t2);
        ray.intersection = ray.origin + t * ray.direction;

        return true;

    }

};

struct Cylinder : ImplicitShape {

    bool rayIntersectPoint(Ray& ray) {



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
