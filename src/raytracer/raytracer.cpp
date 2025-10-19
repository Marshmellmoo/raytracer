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

RGBA toRGBA(const glm::vec4 &illumination) {

    uint8_t r = (uint8_t)(255.0f * glm::min(glm::max(illumination[0], 0.0f), 1.0f));
    uint8_t g = (uint8_t)(255.0f * glm::min(glm::max(illumination[1], 0.0f), 1.0f));
    uint8_t b = (uint8_t)(255.0f * glm::min(glm::max(illumination[2], 0.0f), 1.0f));

    return RGBA{r, g, b, 255};

}

bool isReflective(SceneMaterial& mat) {
    return mat.cReflective.r > 0.0f ||
           mat.cReflective.g > 0.0f ||
           mat.cReflective.b > 0.0f;
}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {

    std::vector<Ray> rays = std::vector<Ray>();
    for (int j = 0; j < scene.height(); j++) {
        for (int i = 0; i < scene.width(); i++) {

            Ray ray = scene.getCamera().generateRay(j, i);
            glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
            glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

            ray.origin    = glm::vec3(originWorld);
            ray.direction = glm::normalize(glm::vec3(directionWorld));

            imageData[pointToIndex(i, j, scene.width())] = toRGBA(traceRay(ray, scene, 0));

        }
    }

}

// Should return an RGBA value.
glm::vec4 RayTracer::traceRay(Ray ray,
                              const RayTraceScene &scene,
                              int recursiveDepth) {

    float t;
    glm::vec3 hitPoint;
    glm::vec3 hitPointObject; // store the object-space hit for chosen shape
    glm::vec3 normal;
    float smallestT = INFINITY;

    std::shared_ptr<Shape> closestShape = nullptr;

    // Object Intersection Checking --
    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject    = glm::vec3(shape->inverseCTM * glm::vec4(ray.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(shape->inverseCTM * glm::vec4(ray.direction, 0.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint)) {

            glm::vec3 hitPointWorld = glm::vec3(shape->shapeInfo.ctm * glm::vec4(hitPoint, 1.0f));
            float tWorld = glm::length(hitPointWorld - ray.origin);

            if (tWorld < smallestT && tWorld > 1e-6f) {

                smallestT = tWorld;
                closestShape = shape;
                normal = shape->computeNormal(hitPoint);
                hitPointObject = hitPoint;

            };

        }

    }

    // Color Calculations --

    t = smallestT;
    glm::vec4 color;

    if (std::isinf(t)) {

        return glm::vec4(0, 0, 0, 1);

    } else {

        glm::vec3 normalWorld = glm::vec3(glm::sign(glm::determinant(glm::mat3(closestShape->shapeInfo.ctm))) * glm::transpose(glm::mat3(closestShape->inverseCTM)) * normal);
        glm::vec3 hitPointWorld = glm::vec3(closestShape->shapeInfo.ctm * glm::vec4(hitPointObject, 1.0f));

        color = phong(hitPointWorld,
                      normalWorld,
                      -ray.direction,
                      scene,
                      closestShape,
                      scene.getLightData());

        // May need to change to qsettings config ?
        if (recursiveDepth < 4 && isReflective(closestShape->shapeInfo.primitive.material)) {

            Ray reflectedRay;
            SceneMaterial material = closestShape->shapeInfo.primitive.material;

            normalWorld = glm::normalize(normalWorld);
            glm::vec3 reflectedDirection = glm::reflect(ray.direction, normalWorld);

            reflectedRay.origin = hitPointWorld + normalWorld * 0.001f;
            reflectedRay.direction = glm::normalize(reflectedDirection);

            glm::vec4 reflectedColor = traceRay(reflectedRay, scene, recursiveDepth + 1);

            glm::vec3 reflWeight = glm::vec3(material.cReflective) * scene.getGlobalData().ks;
            color += glm::vec4(reflWeight * glm::vec3(reflectedColor), 0.0f);

       }

    }

    return color;

}

// Returns true if shadow ray does not intersect with anything before reaching light -- false otherwise.
// Should only be used by phong().
bool traceShadowRay(glm::vec3 position,
                    const RayTraceScene& scene,
                    const SceneLightData &light,
                    glm::vec3 normal) {

    Ray shadowRay;
    shadowRay.origin = position + 0.001f * normal;

    if (light.type == LightType::LIGHT_POINT) shadowRay.direction = glm::normalize((glm::vec3(light.pos) - position));
    else shadowRay.direction = glm::normalize(-glm::vec3(light.dir));

    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject    = glm::vec3(shape->inverseCTM * glm::vec4(shadowRay.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(shape->inverseCTM * glm::vec4(shadowRay.direction, 0.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};
        float t; glm::vec3 hitPoint;

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint)) {

            // If shape intersected with before light, return false.
            if (light.type == LightType::LIGHT_DIRECTIONAL) {

                if (t > 0.0f) return false;

            } else {

                glm::vec3 lightPosObject = glm::vec3(shape->inverseCTM * glm::vec4(glm::vec3(light.pos), 1.0f));

                if (t < glm::length(lightPosObject - hitPoint)) {
                    return false;
                }

            }

        }

    }

    return true;

}

glm::vec4 RayTracer::phong(glm::vec3  position,
           glm::vec3  normal,
           glm::vec3  directionToCamera,
           const RayTraceScene& scene,
           std::shared_ptr<Shape>  &shape,
           const std::vector<SceneLightData> &lights) {

    // Computing Normals
    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    SceneMaterial material = shape->shapeInfo.primitive.material;
    glm::vec3 lightDirection;
    glm::vec4 illumination(0, 0, 0, 1);

    // Ambience --
    glm::vec4 ambience = scene.getGlobalData().ka * material.cAmbient;
    illumination += ambience;

    for (const SceneLightData &light : lights) {

        glm::vec4 diffuse, specular;
        float attenuation = 1.0f;
        float falloff = 1.0f;

        // Shadow Ray Checking --
        if (traceShadowRay(position, scene, light, normal)) {

            if (light.type == LightType::LIGHT_DIRECTIONAL) {

                // Directional Light Calculations --
                attenuation = 1.0f;
                falloff = 1.0f;
                lightDirection = glm::normalize(-glm::vec3(light.dir));

            } else {

                float distance = glm::length(glm::vec3(light.pos) - position);
                attenuation = 1.0f / (light.function.x + distance * light.function.y +
                                      (distance * distance) * light.function.z);
                attenuation = glm::min(1.0f, attenuation);

                lightDirection = glm::normalize((glm::vec3(light.pos) - position));

                if (light.type == LightType::LIGHT_POINT) {

                    // Point Light Calculations --
                    falloff = 1.0f;

                } else {

                    // Spotlight Calculations --

                    float innerAngle = light.angle - light.penumbra;
                    glm::vec3 lightToPoint = glm::normalize(position - glm::vec3(light.pos));
                    float x = glm::acos(glm::dot(glm::normalize(glm::vec3(light.dir)), lightToPoint));

                    if (x <= innerAngle) {

                        falloff = 1.0f;

                    } else if (x <= light.angle) {

                        float a = (x - innerAngle) / (light.angle - innerAngle);
                        falloff = 1.0f - (-2.0f * (a * a * a) + 3.0f * (a * a));

                    } else {

                        falloff = 0.0f;

                    }

                }

            }

            float ndotl = glm::max(glm::dot(normal, lightDirection), 0.0f);

            if (ndotl == 0) {

            }

            // Diffusion Calculations (including UV mapping) --
            if (material.textureMap.isUsed) {

                glm::vec3 positionObject = shape->inverseCTM * glm::vec4(position, 1.0f);
                glm::vec2 uv = shape->computeUV(positionObject);

                Image* textureMap = shape->textureMap;
                int x, y;

                auto textureInfo = shape->shapeInfo.primitive.material.textureMap;

                // Repeating texture checking, normal assignment otherwise
                if (textureInfo.repeatU > 0) x = (int)(glm::floor(uv.x * textureInfo.repeatU * textureMap->width)) % textureMap->width;
                else x =(int)(uv.x * textureMap->width);

                if (textureInfo.repeatV > 0) y = (int)(glm::floor((1 - uv.y) * textureInfo.repeatV * textureMap->height)) % textureMap->height;
                else y = (int)((1- uv.y) * textureMap->height);

                // Bounds checking
                if (x >= textureMap->width) x = textureMap->width - 1;
                if (y >= textureMap->height) y = textureMap->height - 1;

                RGBA textureColor = textureMap->data[pointToIndex(x, y, textureMap->width)];
                glm::vec4 calc = (scene.getGlobalData().kd * glm::vec4((float)textureColor.r, (float)textureColor.g, (float)textureColor.b, 255.0f)) / 255.0f;
                diffuse = calc * ndotl * material.cDiffuse;

            } else {

                // Normal diffuse assignment
                diffuse = scene.getGlobalData().kd * material.cDiffuse * ndotl;

            }


            // Specular Calculations --
            glm::vec3 projection = glm::reflect(-lightDirection, normal);
            float dotVal = glm::dot(projection, directionToCamera);
            float specPower = std::pow(glm::max(dotVal, 0.0f), material.shininess);
            specular = scene.getGlobalData().ks * material.cSpecular * specPower;

            illumination += (attenuation * light.color * falloff) * (diffuse + specular);


        } else {

            continue;

        }

    }

    return illumination;

}

