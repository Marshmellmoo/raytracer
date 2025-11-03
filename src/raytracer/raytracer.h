#pragma once

#include <glm/glm.hpp>
#include "camera/camera.h"
#include "shapes/shape.h"
#include "textures/texture.h"
#include "utils/ini_utils.h"
#include "utils/rgba.h"
#include "utils/scenedata.h"

#define RAY_TRACE_MAX_DEPTH 4
#define RAY_TRACE_DEFAULT_SPP 64

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        TextureFilterType textureFilterType = TextureFilterType::Nearest;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
        int maxRecursiveDepth    = RAY_TRACE_MAX_DEPTH;
        int samplesPerPixel      = RAY_TRACE_DEFAULT_SPP;
        SuperSamplerPattern superSamplerPattern = SuperSamplerPattern::Grid;
        bool onlyRenderNormals   = false;
        bool enableMipMapping    = false;
    };

public:
    RayTracer(Config config);

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(RGBA *imageData, const RayTraceScene &scene);

private:

    const Config m_config;
    int spp;
    int spp_sqrt;

    glm::vec4 raytrace(Ray ray,
                  const RayTraceScene &scene,
                  int recursiveDepth);

    // Change to type Texture
    glm::vec4 texture(Texture texture,
                      std::tuple<glm::vec3, glm::vec3> differentials,
                      glm::vec3 hitPoint,
                      glm::vec3 dp_dx,
                      glm::vec3 dp_dy,
                      std::shared_ptr<Shape>  &shape);

    glm::vec4 phong(glm::vec3  position,
               glm::vec3  normal,
               glm::vec3  directionToCamera,
               const RayTraceScene& scene,
               std::shared_ptr<Shape>  &material,
               const std::vector<SceneLightData> &lights,
               glm::vec4 textureColor);

};

