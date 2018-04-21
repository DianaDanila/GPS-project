//
//  Camera.hpp
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN, ROTATE_RIGHT, ROTATE_LEFT, STAY, ROTATE_UP, ROTATE_DOWN};
    
    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed, float size);
        void rotate(float pitch, float yaw);
		glm::vec3 getCameraDirection();
		glm::vec3 getCameraPosition();
		glm::vec3 getCameraTarget();

		void setCameraPosition(glm::vec3 pos);
		void setCameraTarget(glm::vec3 tar);

		int checkInitial();// 0 for true 1 for false
		int t = 0;
			
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection;
        glm::vec3 cameraRightDirection;
    };
    
}

#endif /* Camera_hpp */
