#include "Collectibles.h"
#include <allegro5/allegro.h>
#include "Game.h"
#include<cstdlib>

Collectible::Collectible(float x, float y, float width, float height)
    : Entity(x, y, width, height), collected(false) {
}

void Collectible::update(float deltaTime) {
}

FuelTank::FuelTank(float x, float y)
    : Collectible(x, y, 30.0f, 30.0f) { // 30x30 size
    image = al_load_bitmap("fueltank.png");
    if (!image) {
		fprintf(stderr, "Failed to load fuel tank image!\n");
		exit(1); 
    }
}

FuelTank::~FuelTank() {
    if (image) al_destroy_bitmap(image);
}

void FuelTank::update(float deltaTime) {}

void FuelTank::render(float cameraX) {
    if (!isCollected() && image) {
        al_draw_bitmap(image, getX() - cameraX, getY(), 0);
    }
}


void FuelTank::applyEffect(Game& game) {
    game.addFuel(60.0f); // Increase fuel by 10%
    collect(); 
}

FuelTank* SpawnFuelTank(const Track& track) {
    float maxTrackX = track.getSegments().back().x2;
    float spawnX = rand() % static_cast<int>(maxTrackX - 100) + 50;
    float spawnY = track.getYAtPosition(spawnX) - 80.0f;
    return new FuelTank(spawnX, spawnY);
}

