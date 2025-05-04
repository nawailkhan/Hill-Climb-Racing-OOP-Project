#include "Car.h"
#include "Physics.h"

Car::Car(float startY, float width, float height) :
    y(startY), width(width), height(height),
    velocityY(0), velocityX(0),
    onGround(false), angle(0), angularVelocity(0), currentSlopeAngle(0),
    slopeResistance(0),
    terrainBumpiness(0),
    m_isFlipped(false),
    flipRecoveryTimer(0),
    flipSeverity(0),
    airControlEffectiveness(1.0f) {
}

void Car::jump() {
    if (onGround) {
        velocityY = Physics::JUMP_FORCE;
        angularVelocity = -4.0f;
        onGround = false;
    }
}

void Car::accelerate(float amount) {
    float adjustedAcceleration = getTerrainAdjustedAcceleration();
    velocityX += amount * adjustedAcceleration;
    velocityX = clamp(velocityX, 0.0f, 92.0f); // Allow higher max speed
}

void Car::applyFriction() {
    Physics::applyFriction(velocityX);
}

void Car::rotate(float direction) {
    angularVelocity += direction * 0.7f;
}

void Car::update(const Track& track, float carX) {
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

void Car::updateTerrainPhysics(const Track& track, float carX) {
    float rearY = track.getYAtPosition(carX);
    float frontY = track.getYAtPosition(carX + width);
    float terrainAngle = atan2(frontY - rearY, width) * 180.0f / 3.14159f;

    terrainResistance = Physics::calculateHillDifficulty(terrainAngle) *
        Physics::HILL_CLIMB_RESISTANCE;

    flipRisk = Physics::calculateFlipRisk(velocityX, terrainAngle);

    velocityX *= (1.0f - terrainResistance);
}

void Car::checkFlipCondition() {
    if (std::abs(angle) > Physics::FLIP_THRESHOLD){
        m_isFlipped = true;
        velocityX *= 0.5f; // Slow down when flipping
    }
}

float Car::getCurrentFuelConsumption() const {
    float base = Physics::BASE_FUEL_CONSUMPTION;
    float hillFactor = 1.0f + (terrainResistance * Physics::HILL_FUEL_MULTIPLIER);
    return base * hillFactor * std::abs(velocityX);
}
void Car::analyzeTerrain(const Track& track, float xPos) {
    float rearY = track.getYAtPosition(xPos);
    float frontY = track.getYAtPosition(xPos + width);

    currentSlopeAngle = atan2(frontY - rearY, width) * 180.0f / ALLEGRO_PI;

    // Increase resistance uphill, reduce it downhill
    if (currentSlopeAngle > 0) { // Uphill
        slopeResistance = std::abs(sin(currentSlopeAngle * ALLEGRO_PI / 180.0f)) * 1.5f; // Amplify uphill resistance
    }
    else { // Downhill
        slopeResistance = std::abs(sin(currentSlopeAngle * ALLEGRO_PI / 180.0f)) * 0.5f; // Reduce downhill resistance
    }

    // Bumpiness
    float nextFrontY = track.getYAtPosition(xPos + width + 20.0f);
    terrainBumpiness = std::abs(nextFrontY - frontY) / 20.0f;
}

float Car::getTerrainAdjustedAcceleration() const {
    float basePower = 0.35f;

    // Reduce acceleration uphill, increase it downhill
    float slopeEffect = (currentSlopeAngle > 0)
        ? 1.0f - (slopeResistance * 0.7f) // Uphill: reduce power
        : 1.0f + (std::abs(currentSlopeAngle) * 0.05f); // Downhill: increase power

    return basePower * std::max(0.3f, slopeEffect); // Never below 30% power
}

float Car::getSlopeResistance() const {
    return slopeResistance;
}

float Car::getAirControl() const {
    // Air control reduces when car is rotating fast
    float rotationDamping = 1.0f - (std::abs(angularVelocity) / 20.0f);
    return std::max(0.2f, rotationDamping); // Always some control
}

bool Car::shouldFlip() const {
    float angleRisk = std::abs(angle) / 90.0f; // 0-1+
    float velocityRisk = std::abs(velocityX) / 15.0f; // 0-1+
    float bumpRisk = terrainBumpiness * 2.0f;

    return (angleRisk * velocityRisk * (1.0f + bumpRisk)) > 0.7f;
}

float Car::getFlipSeverity() const {
    float base = std::abs(angle) / 180.0f; // 0-1
    return base + (terrainBumpiness * 0.5f);
}

void Car::startFlipRecovery() {
    m_isFlipped = true;
    flipRecoveryTimer = 2.0f; // 2 second recovery
    flipSeverity = getFlipSeverity();
}

bool Car::updateFlipRecovery() {
    if (m_isFlipped) { // Updated to m_isFlipped
        flipRecoveryTimer -= 1.0f / 60.0f;
        if (flipRecoveryTimer <= 0) {
            m_isFlipped = false;
            return true;
        }
    }
    return false;
}

void Car::resetFlip() {
    m_isFlipped = false;
    angle = 0;
    angularVelocity = 0;
    velocityX *= 0.5f; // Slow down after flip
}

void Car::applyImpact(float force) {
    angularVelocity += force * (terrainBumpiness + 0.5f);
    velocityY -= force * 2.0f; // Bump upward
}

