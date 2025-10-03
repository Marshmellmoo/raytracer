#include <stdexcept>
#include "raytracescene.h"
#include "utils/sceneparser.h"
#include "shapes/shape.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData) {
    // Optional TODO: implement this. Store whatever you feel is necessary.
    m_width = width;
    m_height = height;

    globalData = metaData.globalData;
    SceneCameraData camera = metaData.cameraData;
    cam.init(camera, width, height);

    lights = metaData.lights;
    shapes = parseRenderShapeData(metaData.shapes);
}

std::vector<Shape> parseRenderShapeData(std::vector<RenderShapeData> shapeList) {

    std::vector<Shape> shapes = std::vector<Shape>();

    for (const RenderShapeData& shapeData : shapeList) {

        Shape* shape = nullptr;
        switch (shapeData.primitive.type) {
            case PrimitiveType::PRIMITIVE_CUBE:
                shape = new Cube();
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                shape = new Cone();
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                shape = new Cylinder();
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
                shape = new Sphere();
                break;
        }

        shape->shapeInfo = shapeData;
        shapes.push_back(shape);


    }

}

const int& RayTraceScene::width() const {
    return m_width;
}

const int& RayTraceScene::height() const {
    return m_height;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return globalData;
}

const Camera& RayTraceScene::getCamera() const {
    return cam;
}

const std::vector<SceneLightData>& RayTraceScene::getLightData() const {
    return lights;
}

const std::vector<Shape>& RayTraceScene::getShapeData() const {
    return shapes;
}

