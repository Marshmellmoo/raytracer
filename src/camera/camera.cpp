#include <stdexcept>
#include "camera.h"

void Camera::init(const SceneCameraData& camera, int imgWidth, int imgHeight) {

    this->imgWidth = imgWidth;
    this->imgHeight = imgHeight;

    look = camera.look;
    up = camera.up;
    position = camera.pos;

    w = -glm::normalize(look);
    v = glm::normalize(up - (glm::dot(up, w) * w));
    u = glm::cross(w, v);

    aspectRatio = imgWidth / imgHeight;

    heightAngle = camera.heightAngle;
    widthAngle = heightAngle * aspectRatio;
    aperture = camera.aperture;
    focalLength = camera.focalLength;

    viewMatrix = computeViewMatrix();
}

glm::mat4 Camera::computeViewMatrix() {

    glm::vec4 col0 = glm::vec4(u[0], v[0], w[0], 0);
    glm::vec4 col1 = glm::vec4(u[1], v[1], w[1], 0);
    glm::vec4 col2 = glm::vec4(u[2], v[2], w[2], 0);
    glm::vec4 col3 = glm::vec4(0, 0, 0, 1);


    glm::mat4 mRotate = glm::mat4(col0, col1, col2, col3);
    glm::mat4 mTranslate = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -position[0], -position[1], -position[2], 1);

    glm::mat4 mView = mRotate * mTranslate;

    return mView;

}

// Generates camera in CAMERA SPACE.
Ray Camera::generateRay(int i, int j) const {

    float x = 2 * k * glm::tan(heightAngle / 2) * (((j + 0.5) / imgWidth) - 0.5);
    float y = 2 * k * glm::tan(widthAngle / 2) * (((imgHeight - 1 - i + 0.5) / imgHeight) - 0.5);
    float z = -k;

    glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(x, y, z));

    return Ray {rayOrigin, rayDirection};

}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return glm::inverse(viewMatrix);
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
