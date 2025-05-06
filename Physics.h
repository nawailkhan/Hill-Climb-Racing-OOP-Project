#ifndef PHYSICS_H

#define PHYSICS_H



class Physics {

public:

    static const float GRAVITY;

    static const float FRICTION;

    static const float AIR_RESISTANCE;

    static const float JUMP_FORCE;

    static const float ROTATION_DAMPING;



    static void applyGravity(float& velocityY, bool onGround);

    static void applyFriction(float& velocityX);

    static void applyAirResistance(float& velocityX, float& velocityY);

    static bool checkFlipCondition(float angle);

    static float calculateAngularAcceleration(float angle);

};



#endif 
