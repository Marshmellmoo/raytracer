#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"
#include "shapes/shape.h"
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

class RayTraceScene
{

private:

    Camera cam;
    SceneGlobalData globalData;

    int m_width;
    int m_height;

    std::vector<SceneLightData> lights;
    std::vector<std::shared_ptr<Shape>> shapes;

public:
    RayTraceScene(int width, int height, const RenderData &metaData);

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of light data of the scene
    const std::vector<SceneLightData>& getLightData() const;

    // The getter of the shape data scene
    const std::shared_ptr<Shape>& getShapeData() const;

    // The getter of the shared pointer to the camera instance of the scene
    const Camera& getCamera() const;

    std::vector<std::shared_ptr<Shape*>> parseRenderShapeData(std::vector<RenderShapeData> shapeList);
};
