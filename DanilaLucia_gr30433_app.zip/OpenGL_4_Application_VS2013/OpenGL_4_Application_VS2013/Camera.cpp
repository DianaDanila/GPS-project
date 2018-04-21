//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {
	float angle = 0.5f;
	
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , glm::vec3(0.0f, 1.0f, 0.0f));
    }

	int Camera::checkInitial(){
		if (this->cameraPosition == glm::vec3(9.5f, 0.0f, 9.5f))
			if (this->cameraTarget == glm::vec3(0.0f, 0.0f, 0.0f))
				return 0;
		return 1;
	}
    
    void Camera::move(MOVE_DIRECTION direction, float speed, float size)
    {
		glm::vec3 temporary = cameraPosition;
        switch (direction) {
            case MOVE_FORWARD:
				temporary += cameraDirection * speed;
                break;
                
            case MOVE_BACKWARD:
				temporary -= cameraDirection * speed;
                break;
                
            case MOVE_RIGHT:
				temporary += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
				temporary -= cameraRightDirection * speed;
                break;

			case MOVE_UP:
				temporary += glm::cross(cameraRightDirection, cameraDirection) * speed;
				break;

			case MOVE_DOWN:
				temporary -= glm::cross(cameraRightDirection, cameraDirection) * speed;
				break;

			case ROTATE_LEFT:
				cameraDirection = glm::rotate(glm::radians(angle), glm::vec3(0, 1, 0)) * glm::vec4(cameraDirection, 1.0f);
				cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
				break;

			case ROTATE_RIGHT:
				cameraDirection = glm::rotate(glm::radians(-angle), glm::vec3(0, 1, 0)) * glm::vec4(cameraDirection, 1.0f);
				cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
				break;

			case STAY:
				break;

			case ROTATE_UP:
				cameraDirection = glm::rotate(glm::radians(angle), cameraRightDirection) * glm::vec4(cameraDirection, 0.0f);
				cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
				break;

			case ROTATE_DOWN:
				cameraDirection = glm::rotate(glm::radians(-angle), cameraRightDirection) * glm::vec4(cameraDirection, 0.0f);
				cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
				break;

        }
		
		if (temporary.x > size - 0.1f)
			temporary.x = cameraPosition.x;
		if (temporary.x < -size + 0.1f)
			temporary.x = cameraPosition.x;
		if (temporary.y > 10.0f)
			temporary.y = cameraPosition.y;
		if (temporary.y < -0.0f && t == 0)
			temporary.y = cameraPosition.y;
		if (temporary.z > size - 0.1f){
			//temporary.z = cameraPosition.z;
			temporary.y -= speed;
			t = 1;
		}
		if (temporary.z < -size + 0.1f)
			temporary.z = cameraPosition.z;
		if (t == 1 && checkInitial())
			temporary.y -= speed;

		
		float rockSize = 4.5f;
		if (temporary.x < rockSize + 0.5f && temporary.x > rockSize - 0.5f && temporary.z < rockSize + 0.5f && temporary.z > rockSize - 0.5f){
			temporary.x = cameraPosition.x;
			temporary.z = cameraPosition.z;
			}

		

		cameraPosition = temporary;

    }
    

	//yaw around y ==> pitch around x
	void Camera::rotate(float pitch, float yaw)
	{
		cameraDirection = glm::rotate(glm::radians(yaw), glm::cross(cameraDirection, cameraRightDirection)) * glm::vec4(cameraDirection, 0.0f);
		cameraDirection = glm::rotate(glm::radians(pitch), cameraRightDirection) * glm::vec4(cameraDirection, 0.0f);
	}
    
	glm::vec3 Camera::getCameraDirection(){
		return this->cameraDirection;
	}

	glm::vec3 Camera::getCameraPosition(){
		return this->cameraPosition;
	}

	glm::vec3 Camera::getCameraTarget(){
		return this->cameraTarget;
	}

	void Camera::setCameraPosition(glm::vec3 pos){
		this->cameraPosition = pos;
	}

	void Camera::setCameraTarget(glm::vec3 tar){
		this->cameraTarget = tar;
	}
}
