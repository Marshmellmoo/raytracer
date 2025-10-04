#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

glm::mat4 getScalingMatrix(float sx, float sy, float sz) {
    // Task 1: return the transformation matrix for 3D scaling
    glm::mat4 scalingMat = glm::mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
    // Task 4: modify the function to return a 4x4 matrix
    return scalingMat;
}

glm::mat4 getRotationMatrixX(float angle) {
    // Task 2: return the matrix for rotation about the x-axis
    // float rad = glm::radians(angle);
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    glm::mat4 rotationMatX = glm::mat4(1, 0, 0, 0, 0, cos, sin, 0, 0, -sin, cos, 0, 0, 0, 0, 1);

    // Task 4: modify the function to return a 4x4 matrix
    return rotationMatX;
}

glm::mat4 getRotationMatrixY(float angle) {
    // Task 2: return the matrix for rotation about the y-axis
    // float rad = glm::radians(angle);
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    glm::mat4 rotationMatY = glm::mat4(cos, 0, -sin, 0, 0, 1, 0, 0, sin, 0, cos, 0, 0, 0, 0, 1);

    // Task 4: modify the function to return a 4x4 matrix
    return rotationMatY;
}

glm::mat4 getRotationMatrixZ(float angle) {
    // Task 2: return the matrix for rotation about the z-axis
    // float rad = glm::radians(angle);
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    glm::mat4 rotationMatZ = glm::mat4(cos, sin, 0, 0, -sin, cos, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

    // Task 4: modify the function to return a 4x4 matrix
    return rotationMatZ;
}

glm::mat4 getTranslationMatrix(float dx, float dy, float dz) {
    // Task 3: return the matrix for translation
    glm::mat4 transformMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, dx, dy, dz, 1);
    return transformMat;
}

void nodeTraversal(SceneNode* node, RenderData &renderData, glm::mat4 ctm) {

    if (node == NULL) {
        return;
    }

    glm::mat4 culCTM = glm::mat4(1.0f);

    for (SceneTransformation* transform : node->transformations) {
        switch (transform->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            culCTM *= glm::translate(glm::mat4(1.0f), transform->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            culCTM *= glm::rotate(glm::mat4(1.0f), transform->angle, transform->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            culCTM *= glm::scale(glm::mat4(1.0f), transform->scale);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            culCTM *= transform->matrix;
            break;
        }
    }

    ctm *= culCTM;

    for (ScenePrimitive* prim : node->primitives) {
        RenderShapeData primitive = {*prim, ctm};
        renderData.shapes.push_back(primitive);
    }

    for (SceneLight* light : node->lights) {
        glm::vec4 lightPos = {0, 0, 0, 1};
        SceneLightData lighting = {light->id, light->type, light->color, light->function, lightPos * ctm, light->dir * ctm, light->penumbra, light->angle, light->width, light->height};
        renderData.lights.push_back(lighting);
    }

    for (SceneNode* child : node->children) {
        nodeTraversal(child, renderData, ctm);
    }

}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);

    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    auto rootNode = fileReader.getRootNode();
    renderData.shapes.clear();

    nodeTraversal(rootNode, renderData, glm::mat4(1.0f));

    return true;

}


