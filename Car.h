#ifndef CAR_H

#define CAR_H



#include "Physics.h"

#include "Track.h"



class Car {

private:

    float y;

    float width;

    float height;

    float velocityY;

    float velocityX;

    bool onGround;

    float angle;

    float angularVelocity;

    float getTerrainAdjustedAcceleration() const;



public:

    Car(float startY, float width = WIDTH_CAR, float height = HEIGHT_CAR);



    void jump();

    void accelerate(float amount = 0.35f);

    void applyFriction();

    void rotate(float direction);

    void update(const Track& track, float carX);



    float getY() const { return y; }

    float getVelocityY() const { return velocityY; }

    float getVelocityX() const { return velocityX; }

    float getAngle() const { return angle; }

    bool isOnGround() const { return onGround; }



    void setY(float newY) { y = newY; }

    void setVelocityY(float newVelocity) { velocityY = newVelocity; }

    void setVelocityX(float newVelocity) { velocityX = newVelocity; }

    void setOnGround(bool ground) { onGround = ground; }

};



#endif
