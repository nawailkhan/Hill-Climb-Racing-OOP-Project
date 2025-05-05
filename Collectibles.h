#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "Entity.h"

class Collectible : public Entity {
private:
    bool collected;

public:
    Collectible(float x, float y, float width, float height)
        : Entity(x, y, width, height), collected(false) {
    }

    void update(float collectible) override {
        // to be updated when a coin or fuel collides with the car

    }

    void collect() { collected = true; }
    bool isCollected() const { return collected; }
};

#endif
