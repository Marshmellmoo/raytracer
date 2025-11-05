#include <stdexcept>
#include <glm/glm.hpp>
#include "camera.h"
#include <tuple>

void Camera::init(const SceneCameraData& camera, int imgWidth, int imgHeight) {

    this->imgWidth = imgWidth;
    this->imgHeight = imgHeight;

    look = camera.look;
    up = camera.up;
    position = camera.pos;

    w = -glm::normalize(look);
    v = glm::normalize(up - (glm::dot(up, w) * w));
    u = glm::cross(v, w);

    aspectRatio = (float)imgWidth / (float)imgHeight;

    heightAngle = camera.heightAngle;
    widthAngle = 2.0f * atan(aspectRatio * tan(heightAngle / 2.0f));

    aperture = camera.aperture;
    focalLength = camera.focalLength;

    viewMatrix = computeViewMatrix();
    inverseViewMatrix = glm::inverse(viewMatrix);

}

glm::mat4 Camera::computeViewMatrix() {

    glm::vec4 col0 = glm::vec4(u[0], v[0], w[0], 0);
    glm::vec4 col1 = glm::vec4(u[1], v[1], w[1], 0);
    glm::vec4 col2 = glm::vec4(u[2], v[2], w[2], 0);
    glm::vec4 col3 = glm::vec4(0, 0, 0, 1);


    glm::mat4 mRotate = glm::mat4(col0, col1, col2, col3);
    glm::mat4 mTranslate = glm::mat4(1, 0, 0, 0,
                                     0, 1, 0, 0,
                                     0, 0, 1, 0,
                                     -position[0], -position[1], -position[2], 1);

    glm::mat4 mView = mRotate * mTranslate;

    return mView;

}

// Generates ray in CAMERA SPACE.
Ray Camera::generateRay(float i, float j) const {

    float x = 2 * k * glm::tan(widthAngle / 2) * ((j / imgWidth) - 0.5);
    float y = 2 * k * glm::tan(heightAngle / 2) * (((imgHeight - 1 - i) / imgHeight) - 0.5);
    float z = -k;

    glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
    glm::vec3 rayDirection = glm::vec3(x, y, z);

    return Ray {rayOrigin, rayDirection};

}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseViewMatrix;
}

float Camera::getAspectRatio() const {
    return aspectRatio;
}

float Camera::getHeightAngle() const {
    return heightAngle;
}

float Camera::getFocalLength() const {
    return focalLength;
}

float Camera::getAperture() const {
    return aperture;
}

void Camera::calculateR(int spp) const {

    float sWidth  = (static_cast<float>(imgWidth) * glm::ceil(glm::sqrt(static_cast<float>(spp))));
    float sHeight = (static_cast<float>(imgHeight) * glm::ceil(glm::sqrt(static_cast<float>(spp))));

    float x = (2.0f * k * glm::tan(widthAngle / 2.0f)) / sWidth;
    float y = (2.0f * k * glm::tan(heightAngle / 2.0f)) / sHeight;

    std::get<0>(r_bar) = glm::vec3(x, 0.0f, 0.0f);
    std::get<1>(r_bar) = glm::vec3(0.0f, y, 0.0f);

}
