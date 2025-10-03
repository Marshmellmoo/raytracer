#include "raytracer.h"
#include "raytracescene.h"
#include "shapes/shape.h"

RayTracer::RayTracer(Config config) :
    m_config(config)
{}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`

    // TODO: Implement the ray tracing algorithm. Good luck!
    std::vector<Ray> rays = std::vector<Ray>();
    for (int j = 0; j < scene.height(); j++) {
        for (int i = 0; j < scene.width(); j++) {

            Ray ray = scene.getCamera().generateRay(i, j);
            glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
            glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 1.0f);

            ray.origin = glm::vec3(originWorld);
            ray.direction = glm::normalize(glm::vec3(directionWorld));

            float t;
            glm::vec3 hitPoint;
            glm::vec3 normal;

            std::shared_ptr<Shape> intersections = traceRay(ray, scene, t, hitPoint, normal);

            if (t == INFINITY) {

                imageData[i] = RGBA{0, 0, 0, 255};

            } else {



            }

        }
    }

}

std::shared_ptr<Shape> RayTracer::traceRay(Ray ray, const RayTraceScene &scene, float& t, glm::vec3& hitPoint, glm::vec3& normal) {

    float smallestT = INFINITY;
    std::shared_ptr<Shape> intersectedShape = nullptr;

    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject = glm::vec3(glm::inverse(shape->shapeInfo.ctm) * glm::vec4(ray.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(glm::inverse(shape->shapeInfo.ctm) * glm::vec4(ray.direction, 1.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint, normal)) {

            if (t < smallestT) {
                smallestT = t;
                intersectedShape = shape;
            };

        }

    }

    return intersectedShape;

}
