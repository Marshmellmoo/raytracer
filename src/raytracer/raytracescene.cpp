#include <stdexcept>
#include "raytracescene.h"
#include "textures/texture.h"
#include "utils/sceneparser.h"
#include "shapes/shape.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/imagereader.h"

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
                cube->inverseCTM = glm::inverse(shapeData.ctm);

                if (cube->shapeInfo.primitive.material.textureMap.isUsed) {

                    Image* txt = loadImageFromFile(cube->shapeInfo.primitive.material.textureMap.filename);
                    SceneFileMap data = cube->shapeInfo.primitive.material.textureMap;
                    float blend = cube->shapeInfo.primitive.material.blend;

                    Texture t = Texture(txt, data, blend);
                    cube->texture = t;
                    cube->texture.generateMaps();

                }

                shapes.push_back(cube);
                break;

            }

            case PrimitiveType::PRIMITIVE_CONE: {

                std::shared_ptr<Shape> cone  = std::make_shared<Cone>();
                cone->shapeInfo = shapeData;
                cone->inverseCTM = glm::inverse(shapeData.ctm);

                if (cone->shapeInfo.primitive.material.textureMap.isUsed) {

                    Image* txt = loadImageFromFile(cone->shapeInfo.primitive.material.textureMap.filename);
                    SceneFileMap data = cone->shapeInfo.primitive.material.textureMap;
                    float blend = cone->shapeInfo.primitive.material.blend;

                    Texture t = Texture(txt, data, blend);
                    cone->texture = t;
                    cone->texture.generateMaps();

                }

                shapes.push_back(cone);
                break;

            }

            case PrimitiveType::PRIMITIVE_CYLINDER: {

                std::shared_ptr<Shape> cyl  = std::make_shared<Cylinder>();
                cyl->shapeInfo = shapeData;
                cyl->inverseCTM = glm::inverse(shapeData.ctm);

                if (cyl->shapeInfo.primitive.material.textureMap.isUsed) {

                    Image* txt = loadImageFromFile(cyl->shapeInfo.primitive.material.textureMap.filename);
                    SceneFileMap data = cyl->shapeInfo.primitive.material.textureMap;
                    float blend = cyl->shapeInfo.primitive.material.blend;

                    Texture t = Texture(txt, data, blend);
                    cyl->texture = t;
                    cyl->texture.generateMaps();

                }

                shapes.push_back(cyl);
                break;

            }

            case PrimitiveType::PRIMITIVE_SPHERE: {

                std::shared_ptr<Shape> sphere  = std::make_shared<Sphere>();
                sphere->shapeInfo = shapeData;
                sphere->inverseCTM = glm::inverse(shapeData.ctm);

                if (sphere->shapeInfo.primitive.material.textureMap.isUsed) {

                    Image* txt = loadImageFromFile(sphere->shapeInfo.primitive.material.textureMap.filename);
                    SceneFileMap data = sphere->shapeInfo.primitive.material.textureMap;
                    float blend = sphere->shapeInfo.primitive.material.blend;

                    Texture t = Texture(txt, data, blend);
                    sphere->texture = t;
                    sphere->texture.generateMaps();

                }

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

