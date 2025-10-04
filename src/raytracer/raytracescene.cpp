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

std::vector<std::shared_ptr<Shape>> RayTraceScene::parseRenderShapeData(std::vector<RenderShapeData> shapeList) {

    std::vector<std::shared_ptr<Shape>> shapes = std::vector<std::shared_ptr<Shape>>();

    for (const RenderShapeData& shapeData : shapeList) {

        switch (shapeData.primitive.type) {

            case PrimitiveType::PRIMITIVE_CUBE: {

                std::shared_ptr<Shape> cube  = std::make_shared<Cube>();
                cube->shapeInfo = shapeData;
                shapes.push_back(cube);
                break;

            }

            case PrimitiveType::PRIMITIVE_CONE: {

                std::shared_ptr<Shape> cone  = std::make_shared<Cone>();
                cone->shapeInfo = shapeData;
                shapes.push_back(cone);
                break;

            }

            case PrimitiveType::PRIMITIVE_CYLINDER: {

                std::shared_ptr<Shape> cyl  = std::make_shared<Cylinder>();
                cyl->shapeInfo = shapeData;
                shapes.push_back(cyl);
                break;

            }

            case PrimitiveType::PRIMITIVE_SPHERE: {

                std::shared_ptr<Shape> sphere  = std::make_shared<Sphere>();
                sphere->shapeInfo = shapeData;
                shapes.push_back(sphere);
                break;

            }

            case PrimitiveType::PRIMITIVE_MESH:
                break;
            }

    }

    return shapes;

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

const std::vector<std::shared_ptr<Shape>>& RayTraceScene::getShapeData() const {
    return shapes;
}

