#ifndef GAME_H
#define GAME_H
#include <allegro5/allegro.h>
#include "Resources.h"
#include "Car.h"
#include "Track.h"
#include "Collectibles.h"
class Game {
private:
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* font;
    ALLEGRO_BITMAP* sky;
    ALLEGRO_BITMAP* car_image;
    bool running;
    bool gameOverShown;
    bool key[ALLEGRO_KEY_MAX];
    float cameraX;
    float fuel;
    Car car;
    Track track;
    FuelTank* fuelTank = nullptr;
    
    void initialize();
    void handleEvents();
    void update();
    void render();
    void cleanUp();
public:
    Game();
    void addFuel(float amount);
    ~Game();
    void run();
};
#endif 

