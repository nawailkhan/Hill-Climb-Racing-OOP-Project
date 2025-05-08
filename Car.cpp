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
    // Calculate acceleration adjustment based on terrain angle
    // Positive angle means uphill (slower acceleration)
    // Negative angle means downhill (faster acceleration)
    float slopeEffect = -sin(angle * ALLEGRO_PI / 180.0f);

    // Base multiplier (1.0) plus slope effect
    // Downhill (negative angle) will increase acceleration
    // Uphill (positive angle) will decrease acceleration
    return 1.0f + slopeEffect * 0.8f; // 0.8 controls the intensity of the effect
}

void Car::accelerate(float amount) {
    // Reduce the base acceleration amount to slow down the car overall
    float reducedAmount = amount * 0.25f; // 60% of original acceleration

    float adjustedAcceleration = getTerrainAdjustedAcceleration();
    velocityX += reducedAmount * adjustedAcceleration;

    // Add velocity clamping back to control maximum speed
    // Lower the maximum speed limit
    const float MAX_SPEED = 15.0f; // Lower than the original implicit limit
    if (velocityX > MAX_SPEED) velocityX = MAX_SPEED;
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
    float rearY = track.getYAtPosition(carX);
    float frontY = track.getYAtPosition(carX + width);
    currentSlopeAngle = atan2(frontY - rearY, width) * 180.0f / ALLEGRO_PI;

    // Then apply physics
    Physics::applyAirResistance(velocityX, velocityY, currentSlopeAngle);

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

    y = avgY - height -6.0f;
    angle = desiredAngle;
    angularVelocity = 0;
    velocityY = 0;
    onGround = true;
}
