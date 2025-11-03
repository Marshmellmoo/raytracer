#pragma once

#include "utils/scenedata.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

struct Ray {

    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 unnormalizedDirection;

};

class Camera {
private:

    int imgWidth;
    int imgHeight;

    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;
    float k = 1.0f;

    glm::vec3 look;
    glm::vec3 up;

    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;

    float aspectRatio;

    float heightAngle;
    float widthAngle;
    float aperture;
    float focalLength;

    glm::mat4 computeViewMatrix();

public:

    mutable std::tuple<glm::vec3, glm::vec3> r_bar;
    glm::vec3 position;

    void init(const SceneCameraData& camera, int imgWidth, int imgHeight);

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    Ray generateRay(int i, int j) const;
    void calculateR(int spp) const;

};
