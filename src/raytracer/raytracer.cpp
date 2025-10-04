#include "raytracer.h"
#include "raytracescene.h"
#include "shapes/shape.h"
#include <iostream>

RayTracer::RayTracer(Config config) :
    m_config(config)
{}

int pointToIndex(int i, int j, int width) {

    return j * width + i;

}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`

    // TODO: Implement the ray tracing algorithm. Good luck!
    std::vector<Ray> rays = std::vector<Ray>();
    for (int j = 0; j < scene.height(); j++) {
        for (int i = 0; i < scene.width(); i++) {

            if (j == scene.height() / 2 && i == scene.width() / 2) {

            }

            Ray ray = scene.getCamera().generateRay(j, i);
            glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
            glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

            ray.origin    = glm::vec3(originWorld);
            ray.direction = glm::normalize(glm::vec3(directionWorld));

            float t;
            glm::vec3 hitPoint;
            glm::vec3 normal;

            std::shared_ptr<Shape> shape = traceRay(ray, scene, t, hitPoint, normal);

            if (std::isinf(t)) {

                imageData[pointToIndex(i, j, scene.width())] = RGBA{0, 0, 0, 255};

            } else {

                glm::vec3 normalWorld = glm::vec3(glm::sign(glm::determinant(glm::mat3(shape->shapeInfo.ctm))) * glm::transpose(glm::inverse(glm::mat3(shape->shapeInfo.ctm))) * normal);
                glm::vec3 hitPointWorld = glm::vec3(shape->shapeInfo.ctm * glm::vec4(hitPoint, 1.0f));

                normalWorld = glm::normalize(normalWorld);

                imageData[pointToIndex(i, j, scene.width())] = phong(hitPointWorld,
                                                                     normalWorld,
                                                                     -ray.direction,
                                                                     scene.getGlobalData(),
                                                                     shape->shapeInfo.primitive.material,
                                                                     scene.getLightData());

            }

        }
    }

}

std::shared_ptr<Shape> RayTracer::traceRay(Ray ray,
                                           const RayTraceScene &scene,
                                           float& t,
                                           glm::vec3& hitPoint,
                                           glm::vec3& normal) {

    float bestT;
    glm::vec3 bestHitPoint;
    glm::vec3 bestNormal;

    float smallestT = INFINITY;

    std::shared_ptr<Shape> closestIntersectedShape = nullptr;

    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject    = glm::vec3(glm::inverse(shape->shapeInfo.ctm) * glm::vec4(ray.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(glm::inverse(shape->shapeInfo.ctm) * glm::vec4(ray.direction, 0.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint, normal)) {

            if (t < smallestT) {

                smallestT = t;
                closestIntersectedShape = shape;

                bestT = t;
                bestHitPoint = hitPoint;
                bestNormal = normal;

            };

        }

    }

    t = smallestT;
    hitPoint = bestHitPoint;
    normal = bestNormal;

    return closestIntersectedShape;

}

RGBA toRGBA(const glm::vec4 &illumination) {

    uint8_t r = (uint8_t)(255.0f * glm::min(glm::max(illumination[0], 0.0f), 1.0f));
    uint8_t g = (uint8_t)(255.0f * glm::min(glm::max(illumination[1], 0.0f), 1.0f));
    uint8_t b = (uint8_t)(255.0f * glm::min(glm::max(illumination[2], 0.0f), 1.0f));

    return RGBA{r, g, b, 255};

}

RGBA RayTracer::phong(glm::vec3 position,
           glm::vec3 normal,
           glm::vec3  directionToCamera,
           SceneGlobalData globalData,
           SceneMaterial &material,
           const std::vector<SceneLightData> &lights) {

    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    glm::vec4 illumination(0, 0, 0, 1);

    glm::vec4 ambience = globalData.ka * material.cAmbient;
    illumination += ambience;

    for (const SceneLightData &light : lights) {

        glm::vec3 lightDirection = glm::normalize(-glm::vec3(light.dir));
        float ndotl = glm::max(glm::dot(normal, lightDirection), 0.0f);

        if (ndotl > 0.0f) {

            glm::vec4 diffuse = globalData.kd * material.cDiffuse * ndotl;

            glm::vec3 projection = glm::reflect(-lightDirection, normal);
            float dotVal = glm::dot(projection, directionToCamera);
            float specPower = std::pow(glm::max(dotVal, 0.0f), material.shininess);
            glm::vec4 specular = globalData.ks * material.cSpecular * specPower;

            illumination += (light.color) * (diffuse + specular);

        }

    }


    /*    glm::vec3 reflectedRay = -directionToCamera - 2.0f * glm::dot(glm::normalize(-directionToCamera), normal) * normal;
    // Made directionToCamera negative, and normalized directionToCamera in the dot product.
    illumination += kr * reflectionSampler.getReflection(position, reflectedRay); */  // <-- no need to edit this after uncommenting

    RGBA returnValue = toRGBA(illumination);
    return returnValue;

}

