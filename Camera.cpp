#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        //  this->cameraUpDirection = cameraUp;
        this->originalCameraUp = cameraUp;
        this->cameraFrontDirection = glm::normalize(glm::vec3(cameraTarget - cameraPosition));
        this->originalFrontDirection = this->cameraFrontDirection;
        this->cameraRightDirection = (glm::normalize(glm::cross(this->cameraFrontDirection, this->originalCameraUp)));
        this->cameraUpDirection = glm::cross(this->cameraRightDirection, this->cameraFrontDirection);

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(this->cameraPosition, this->cameraTarget, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += this->cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= this->cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
            break;
        }
        this->cameraTarget = cameraPosition + this->cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::mat4 euler = glm::yawPitchRoll(yaw, pitch, 0.0f);

        glm::vec3 resultCameraFrontDirection = glm::mat3(euler) * this->originalFrontDirection;
        this->cameraFrontDirection = (glm::normalize(resultCameraFrontDirection));
        this->cameraRightDirection = (glm::normalize(glm::cross(this->cameraFrontDirection, this->originalCameraUp)));
        this->cameraUpDirection = glm::cross(this->cameraRightDirection, this->cameraFrontDirection);
        this->cameraTarget = cameraPosition + this->cameraFrontDirection;
    }

}