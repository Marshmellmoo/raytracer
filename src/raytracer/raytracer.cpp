#include "raytracer.h"
#include "raytracescene.h"
#include "shapes/shape.h"
#include "textures/texture.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

#include <iostream>

RayTracer::RayTracer(Config config) :
    m_config(config)
{}

//                                                      ===== HELPER FUNCTIONS ======

// Point to index function.
inline int pointToIndex(int i, int j, int width) {

    return j * width + i;

}

// Change vec4 of floats to RGBA value.
inline RGBA toRGBA(const glm::vec4 &illumination) {

    uint8_t r = (uint8_t)(255.0f * glm::min(glm::max(illumination[0], 0.0f), 1.0f));
    uint8_t g = (uint8_t)(255.0f * glm::min(glm::max(illumination[1], 0.0f), 1.0f));
    uint8_t b = (uint8_t)(255.0f * glm::min(glm::max(illumination[2], 0.0f), 1.0f));

    return RGBA{r, g, b, 255};

}

// Returns true if shadow ray does not intersect with anything before reaching light -- false otherwise.
// Should only be used by phong().
bool traceShadowRay(glm::vec3 position,
                    const RayTraceScene& scene,
                    const SceneLightData &light,
                    glm::vec3 normal) {

    Ray shadowRay;
    shadowRay.origin = position + 0.001f * normal;

    if (light.type == LightType::LIGHT_DIRECTIONAL) shadowRay.direction = glm::normalize(-glm::vec3(light.dir));
    else shadowRay.direction = shadowRay.direction = glm::normalize((glm::vec3(light.pos) - position));

    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject    = glm::vec3(shape->inverseCTM * glm::vec4(shadowRay.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(shape->inverseCTM * glm::vec4(shadowRay.direction, 0.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};
        float t; glm::vec3 hitPoint;

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint)) {

            if (light.type == LightType::LIGHT_DIRECTIONAL) {

                if (t > 0.0f) return false; // If any intersection is done in the direction of the light, there should be a shadow.

            } else {

                glm::vec3 lightPosObject = glm::vec3(shape->inverseCTM * glm::vec4(glm::vec3(light.pos), 1.0f));
                // If shape intersected with before light, return false.
                if (t < glm::length(lightPosObject - hitPoint)) {
                    return false;
                }

            }

        }

    }

    return true;

}

// Checks if material is reflective.
bool isReflective(SceneMaterial& mat) {
    return mat.cReflective.r > 0.0f ||
           mat.cReflective.g > 0.0f ||
           mat.cReflective.b > 0.0f;
}





//                                                      ===== MAIN RAYTRACING FUNCTIONS ======
void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {

    // Samples per pixel initializing .
    spp_sqrt = glm::ceil(glm::sqrt(m_config.samplesPerPixel));
    spp = spp_sqrt * spp_sqrt;

    // Choosing which type of sampling to do !
    bool randomSampling = false;
    bool uniformSampling = false;
    bool stratifiedSampling = false;

    switch(m_config.superSamplerPattern) {

    case SuperSamplerPattern::Grid:
        uniformSampling = true;
        break;
    case SuperSamplerPattern::Random:
        randomSampling = true;
        break;
    case SuperSamplerPattern::Stratified:
        stratifiedSampling = true;
        break;
    }

    // For differential calculations.
    scene.getCamera().calculateR(spp);

    for (int j = 0; j < scene.height(); j++) {
        for (int i = 0; i < scene.width(); i++) {

            if (randomSampling) {

                // Random Sampling
                glm::vec4 color = glm::vec4(0.0f);
                for (int sample = 0; sample < spp; sample++) {

                    float px = (float)i + dis(gen);
                    float py = (float)j + dis(gen);

                    Ray ray = scene.getCamera().generateRay(py + 0.5f, px + 0.5f);
                    glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
                    glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

                    ray.origin    = glm::vec3(originWorld);
                    ray.direction = glm::normalize(glm::vec3(directionWorld));
                    ray.unnormalizedDirection = glm::vec3(directionWorld);

                    color += raytrace(ray, scene, 0);

                }

                color = color / (float)spp;
                imageData[pointToIndex(i, j, scene.width())] = toRGBA(color);

            } else if (uniformSampling) {

                // Uniform Sampling ---
                glm::vec4 color = glm::vec4(0.0f);

                for (int iy = 0; iy < spp_sqrt; iy++) {
                    for (int ix = 0; ix < spp_sqrt; ix++) {

                        float jx = (ix + 0.5f) / (float)spp_sqrt;
                        float jy = (iy + 0.5f) / (float)spp_sqrt;

                        float px = (float)i + jx;
                        float py = (float)j + jy;

                        Ray ray = scene.getCamera().generateRay(py - 0.5f, px - 0.5f);
                        glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
                        glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

                        ray.origin    = glm::vec3(originWorld);
                        ray.direction = glm::normalize(glm::vec3(directionWorld));
                        ray.unnormalizedDirection = glm::vec3(directionWorld);

                        color += raytrace(ray, scene, 0);

                    }
                }

                color = color / (float)spp;
                imageData[pointToIndex(i, j, scene.width())] = toRGBA(color);

            } else if (stratifiedSampling) {

                // Stratified Sampling ---
                glm::vec4 color = glm::vec4(0.0f);

                for (int iy = 0; iy < spp_sqrt; iy++) {
                    for (int ix = 0; ix < spp_sqrt; ix++) {

                        float jx = (ix + dis(gen)) / spp_sqrt;
                        float jy = (iy + dis(gen)) / spp_sqrt;

                        float px = (float)i + jx;
                        float py = (float)j + jy;

                        Ray ray = scene.getCamera().generateRay(py + 0.5f, px + 0.5f);
                        glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
                        glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

                        ray.origin    = glm::vec3(originWorld);
                        ray.direction = glm::normalize(glm::vec3(directionWorld));
                        ray.unnormalizedDirection = glm::vec3(directionWorld);

                        color += raytrace(ray, scene, 0);

                    }
                }

                color = color / (float)m_config.samplesPerPixel;
                imageData[pointToIndex(i, j, scene.width())] = toRGBA(color);

            } else {

                // Normal Sampling ---
                Ray ray = scene.getCamera().generateRay(j, i);
                glm::vec4 originWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.origin, 1.0f);
                glm::vec4 directionWorld = scene.getCamera().getInverseViewMatrix() * glm::vec4(ray.direction, 0.0f);

                ray.origin    = glm::vec3(originWorld);
                ray.direction = glm::normalize(glm::vec3(directionWorld));
                ray.unnormalizedDirection = glm::vec3(directionWorld);

                imageData[pointToIndex(i, j, scene.width())] = toRGBA(raytrace(ray, scene, 0));

            }

        }
    }

}

// Should return an RGBA value as vec4 of ints.
glm::vec4 RayTracer::raytrace(Ray ray,
                              const RayTraceScene &scene,
                              int recursiveDepth) {

    float t;
    glm::vec3 hitPoint;
    glm::vec3 hitPointObject;
    glm::vec3 normal;
    float smallestT = INFINITY;

    std::shared_ptr<Shape> closestShape;

    // Object Intersection Checking --
    for (std::shared_ptr<Shape> shape : scene.getShapeData()) {

        glm::vec3 originObject    = glm::vec3(shape->inverseCTM * glm::vec4(ray.origin, 1.0f));
        glm::vec3 directionObject = glm::vec3(shape->inverseCTM * glm::vec4(ray.direction, 0.0f));

        Ray objectSpaceRay = Ray {originObject, directionObject};

        if (shape->rayIntersect(objectSpaceRay, t, hitPoint)) {

            glm::vec3 hitPointWorld = glm::vec3(shape->shapeInfo.ctm * glm::vec4(hitPoint, 1.0f));
            float tWorld = glm::length(hitPointWorld - ray.origin);

            if (tWorld < smallestT && tWorld > 1e-6f) {

                if (shape->pointShapeCollision(scene.getCamera().position)) {
                    normal = -shape->computeNormal(hitPoint);
                } else { normal = shape->computeNormal(hitPoint); }

                smallestT = tWorld;
                closestShape = shape;
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

        glm::vec4 textureColor;

        // Texture Calculations --
        glm::vec3 normalWorld = glm::vec3(glm::sign(glm::determinant(glm::mat3(closestShape->shapeInfo.ctm))) *
                                          glm::transpose(glm::mat3(closestShape->inverseCTM)) * normal);

        if (closestShape->shapeInfo.primitive.material.textureMap.isUsed) {

            // dp_dx and dp_dy calculations
            glm::vec4 rWorldX = scene.getCamera().getInverseViewMatrix() * glm::vec4(std::get<0>(scene.getCamera().r_bar), 0.0f);
            glm::vec4 rWorldY = scene.getCamera().getInverseViewMatrix() * glm::vec4(std::get<1>(scene.getCamera().r_bar), 0.0f);

            glm::vec3 dd_dx = (glm::vec3(rWorldX) * glm::dot(ray.unnormalizedDirection, ray.unnormalizedDirection) -
                               glm::dot(ray.unnormalizedDirection, glm::vec3(rWorldX)) * ray.unnormalizedDirection) /
                               std::pow(glm::dot(ray.unnormalizedDirection, ray.unnormalizedDirection), 3.0f / 2.0f);
            float dt_dx = -(glm::dot(normal, t * dd_dx)) / glm::dot(normalWorld, ray.direction);

            glm::vec3 dd_dy = (glm::vec3(rWorldY) * glm::dot(ray.unnormalizedDirection, ray.unnormalizedDirection) -
                               glm::dot(ray.unnormalizedDirection, glm::vec3(rWorldY)) * ray.unnormalizedDirection) /
                               std::pow(glm::dot(ray.unnormalizedDirection, ray.unnormalizedDirection), 3.0f / 2.0f);
            float dt_dy = -(glm::dot(normal, t * dd_dy)) / glm::dot(normalWorld, ray.direction);

            glm::vec3 dp_dx = t * dd_dx + dt_dx * ray.direction;
            glm::vec3 dp_dy = t * dd_dy + dt_dy * ray.direction;

            // Computing Differentials for Shape --
            std::tuple<glm::vec3, glm::vec3> differentials = closestShape->computeDifferentials(hitPoint);

            textureColor = texture(closestShape->texture, differentials, hitPoint, dp_dx, dp_dy, closestShape);

        }

        glm::vec3 hitPointWorld = glm::vec3(closestShape->shapeInfo.ctm * glm::vec4(hitPointObject, 1.0f));

        color = phong(hitPointWorld,
                      normalWorld,
                      -ray.direction,
                      scene,
                      closestShape,
                      scene.getLightData(),
                      textureColor);

        // Reflective Ray Handling --
        if (recursiveDepth < m_config.maxRecursiveDepth && isReflective(closestShape->shapeInfo.primitive.material)) {

            Ray reflectedRay;
            SceneMaterial material = closestShape->shapeInfo.primitive.material;

            normalWorld = glm::normalize(normalWorld);
            glm::vec3 reflectedDirection = ray.direction - 2.0f * glm::dot(ray.direction, normalWorld) * normalWorld;

            reflectedRay.origin = hitPointWorld + normalWorld * 0.001f;
            reflectedRay.direction = reflectedDirection;
            reflectedRay.unnormalizedDirection = reflectedDirection;

            glm::vec4 reflectedColor = raytrace(reflectedRay, scene, recursiveDepth + 1);
            glm::vec3 reflWeight = glm::vec3(material.cReflective) * scene.getGlobalData().ks;

            color += glm::vec4(reflWeight * glm::vec3(reflectedColor), 0.0f);

       }

    }

    return color;

}

// Should compute mipmapping and return
glm::vec4 RayTracer::texture(Texture texture,
                  std::tuple<glm::vec3, glm::vec3> differentials,
                  glm::vec3 hitPoint,
                  glm::vec3 dp_dx,
                  glm::vec3 dp_dy,
                  std::shared_ptr<Shape> &shape) {

    glm::vec2 uv = shape->computeUV(hitPoint);

    glm::vec3 dp_dxTexture = glm::vec3(shape->inverseCTM * glm::vec4(dp_dx, 0.0f));
    glm::vec3 dp_dyTexture = glm::vec3(shape->inverseCTM * glm::vec4(dp_dy, 0.0f));
    auto textureInfo = shape->texture.info;

    float ds_du, dt_dv;
    float ds_dx, ds_dy;
    float dt_dx, dt_dy;

    ds_du = (textureInfo.repeatU > 0) ? texture.texture->width * textureInfo.repeatU :
                                        texture.texture->width;
    dt_dv = (textureInfo.repeatV > 0) ? texture.texture->height * textureInfo.repeatV :
                                        texture.texture->height;

    ds_dx = glm::dot(dp_dxTexture, std::get<0>(differentials));
    ds_dx = ds_dx * ds_du;

    ds_dy = glm::dot(dp_dyTexture, std::get<0>(differentials));
    ds_dy = ds_dy * ds_du;

    dt_dx = glm::dot(dp_dxTexture, std::get<1>(differentials));
    dt_dx = dt_dx * dt_dv;

    dt_dy = glm::dot(dp_dyTexture, std::get<1>(differentials));
    dt_dy = dt_dy * dt_dv;

    float X = glm::sqrt(ds_dx * ds_dx + dt_dx * dt_dx);
    float Y = glm::sqrt(ds_dy * ds_dy + dt_dy * dt_dy);

    float S = glm::max(X, Y);
    float L = glm::log2(S);

    glm::vec4 color;

    switch (m_config.textureFilterType) {

    case TextureFilterType::Nearest:
        color = texture.sampleNearest(uv);
        break;

    case TextureFilterType::Bilinear:
        color = texture.sampleBilinear(uv, L, m_config.enableMipMapping);
        break;

    case TextureFilterType::Trilinear:
        color = texture.sampleTrilinear(uv, L, m_config.enableMipMapping);
        break;

    }

    return color;
}


glm::vec4 RayTracer::phong(glm::vec3  position,
           glm::vec3  normal,
           glm::vec3  directionToCamera,
           const RayTraceScene& scene,
           std::shared_ptr<Shape>  &shape,
           const std::vector<SceneLightData> &lights,
           glm::vec4 textureColor) {

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

                // Should be doing attenuation calculations if the light is not directional .
                float distance = glm::length(glm::vec3(light.pos) - position);
                attenuation = 1.0f / (light.function.x +
                                      distance * light.function.y +
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

            // Diffusion Calculations (including UV mapping) --
            if (material.textureMap.isUsed) {

                diffuse = (shape->texture.blend * textureColor + (((1.0f - shape->texture.blend) *
                                                                   scene.getGlobalData().kd * material.cDiffuse))) * ndotl;

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

