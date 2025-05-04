#include "Physics.h"
#include "Resources.h" 

const float Physics::GRAVITY = 1.0f;
const float Physics::FRICTION = 0.030f;
const float Physics::AIR_RESISTANCE = 0.99f;
const float Physics::JUMP_FORCE = -15.0f;
const float Physics::ROTATION_DAMPING = 0.93f;

void Physics::applyGravity(float& velocityY, bool onGround) {
    if (!onGround) {
        velocityY += GRAVITY;
    }
}

void Physics::applyFriction(float& velocityX) {
    if (velocityX > 0) {
        velocityX -= FRICTION;
        if (velocityX < 0) velocityX = 0;
    }
}

void Physics::applyAirResistance(float& velocityX, float& velocityY) {
    velocityX *= AIR_RESISTANCE;
    velocityY *= AIR_RESISTANCE;
}

bool Physics::checkFlipCondition(float angle) {
    return std::abs(angle) > 90.0f;
}

float Physics::calculateAngularAcceleration(float angle) {
    return 0.2f * sin(angle * ALLEGRO_PI / 180.0f);
}