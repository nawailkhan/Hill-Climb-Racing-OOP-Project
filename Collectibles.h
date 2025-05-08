#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "Entity.h"
#include <allegro5/allegro.h> // Add this to define ALLEGRO_BITMAP
class Game; // Forward declaration
class Track;
class Collectible : public Entity {
protected:
    bool collected;

public:
    Collectible(float x, float y, float width, float height);
    virtual void update(float deltaTime);
    virtual void render(float cameraX) = 0;
    virtual void applyEffect(Game& game) = 0;
    void collect() { collected = true; }
    bool isCollected() const { return collected; }
};

class FuelTank : public Collectible {
public:
    FuelTank(float x, float y);
    ~FuelTank();
    void update(float deltaTime) override;
    void render(float cameraX) override;
    void applyEffect(Game& game) override;

private:
    ALLEGRO_BITMAP* image = nullptr; // Declare image variable
};
FuelTank* SpawnFuelTank(const Track& track);
#endif
