#ifndef PHYSICS_H

#define PHYSICS_H



class Physics {

public:

    static const float GRAVITY;

    static const float FRICTION;

    static const float AIR_RESISTANCE;

    static const float JUMP_FORCE;

    static const float ROTATION_DAMPING;
    static const float HILL_CLIMB_RESISTANCE; 
    static const float HILL_DESCEND_BOOST;     
    static const float AIR_RESISTANCE;



    static void applyGravity(float& velocityY, bool onGround);

    static void applyFriction(float& velocityX);

    static void applyAirResistance(float& velocityX, float& velocityY, float slopeAngle);

    static bool checkFlipCondition(float angle);

    static float calculateAngularAcceleration(float angle);

};



#endif 
