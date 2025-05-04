#include "Car.h"
#include "Physics.h"

Car::Car(float startY, float width, float height) :
    y(startY), width(width), height(height),
    velocityY(0), velocityX(0),
    onGround(false), angle(0), angularVelocity(0) {
}

void Car::jump() {
    if (onGround) {
        velocityY = Physics::JUMP_FORCE;
        angularVelocity = -4.0f;
        onGround = false;
    }
}

float Car::getTerrainAdjustedAcceleration() const {
    // Example implementation - adjust based on slope
    return std::cos(angle * ALLEGRO_PI / 180.0f);
}

void Car::accelerate(float amount) {
    amount = 0.10f;
    float adjustedAcceleration = getTerrainAdjustedAcceleration();
    velocityX += amount * adjustedAcceleration;
    //velocityX = clamp(velocityX, 0.0f, 92.0f); // Allow higher max speed
}

void Car::applyFriction() {
    Physics::applyFriction(velocityX);
}

void Car::rotate(float direction) {
    angularVelocity += direction * 0.7f;
}

void Car::update(const Track& track, float carX) {
    // Apply physics
    Physics::applyGravity(velocityY, onGround);
    Physics::applyAirResistance(velocityX, velocityY);

    if (!onGround) {
        angularVelocity += Physics::calculateAngularAcceleration(angle);
        angle += angularVelocity;
        angularVelocity *= Physics::ROTATION_DAMPING;
    }
    y += velocityY;

    //track collision
    onGround = false;
    float rearX = carX;
    float frontX = carX + width;

    float trackRearY = track.getYAtPosition(rearX);
    float trackFrontY = track.getYAtPosition(frontX);

    float dx = frontX - rearX;
    float dy = trackFrontY - trackRearY;

    float desiredAngle = atan2(dy, dx) * (180.0f / ALLEGRO_PI);
    float avgY = (trackRearY + trackFrontY) / 2.0f;

    y = avgY - height;
    angle = desiredAngle;
    angularVelocity = 0;
    velocityY = 0;
    onGround = true;
}
