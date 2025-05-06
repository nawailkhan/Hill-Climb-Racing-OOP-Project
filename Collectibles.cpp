#include "Collectibles.h"
#include <allegro5/allegro.h>
#include "Game.h"
#include<cstdlib>

// Collectible base class methods
Collectible::Collectible(float x, float y, float width, float height)
    : Entity(x, y, width, height), collected(false) {
}

void Collectible::update(float deltaTime) {
    // Base update logic (can be overridden)
}

// FuelTank subclass implementation
FuelTank::FuelTank(float x, float y)
    : Collectible(x, y, 30.0f, 30.0f) { // 30x30 size
    image = al_load_bitmap("fueltank.png");
    if (!image) {
        // Handle error (e.g., log message)
    }
}

FuelTank::~FuelTank() {
    if (image) al_destroy_bitmap(image);
}

void FuelTank::update(float deltaTime) {
    // Update logic if needed (e.g., animation)
}

void FuelTank::render() {
    if (!isCollected() && image) {
        al_draw_bitmap(image, getX(), getY(), 0);
    }
}

void FuelTank::applyEffect(Game& game) {
    game.addFuel(10.0f); // Increase fuel by 10%
    collect(); // Mark as collected
}

// Helper function to spawn fuel tank on the track (call in Game initialization)
FuelTank* SpawnFuelTank(const Track& track) {
    float spawnX = rand() % 10000; // Adjust based on track length
    float spawnY = track.getYAtPosition(spawnX) - 30.0f; // Place on track surface
    return new FuelTank(spawnX, spawnY);
}
