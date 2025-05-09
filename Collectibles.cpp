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
    al_play_sample(game.collectSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    collect();
}

FuelTank* SpawnFuelTank(const Track& track) {
    float maxTrackX = track.getSegments().back().x2;
    float spawnX = rand() % static_cast<int>(maxTrackX - 100) + 50;
    float spawnY = track.getYAtPosition(spawnX) - 80.0f;
    return new FuelTank(spawnX, spawnY);
}

Coin::Coin(float x, float y)
    : Collectible(x, y, 30.0f, 30.0f) { // 30x30 size
    image = al_load_bitmap("Coin.jpg");
    if (!image) {
        fprintf(stderr, "Failed to load coin image!\n");
        exit(1);
    }
}

Coin::~Coin() {
    if (image) al_destroy_bitmap(image);
}

void Coin::update(float deltaTime) {}

//void Coin::render(float cameraX) {
//    if (!isCollected() && image) {
//        al_draw_bitmap(image, getX() - cameraX, getY(), 0);
//    }
//}
void Coin::render(float cameraX) {
    if (!collected && image) {
        float scale = 0.12f;  // shrink to 20% of original size
        float w = al_get_bitmap_width(image) * scale;
        float h = al_get_bitmap_height(image) * scale;
        al_draw_scaled_bitmap(image,
            0, 0,
            al_get_bitmap_width(image), al_get_bitmap_height(image),
            x - cameraX, y,
            w, h,
            0);
    }
}


void Coin::applyEffect(Game& game) {
    game.addCoin(5); 
    al_play_sample(game.collectSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    collect();
}

Coin* SpawnCoin(const Track& track) {
    float maxTrackX = track.getSegments().back().x2;
    float spawnX = rand() % static_cast<int>(maxTrackX - 100) + 50;
    float spawnY = track.getYAtPosition(spawnX) - 80.0f;
    return new Coin(spawnX, spawnY);
}
