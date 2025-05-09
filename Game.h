#ifndef GAME_H
#define GAME_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
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
    ALLEGRO_BITMAP* fuelImage;
    ALLEGRO_BITMAP* coinImage;
    ALLEGRO_BITMAP* gameOverImage;

    bool fuelEmpty;
    bool running;
    bool gameOverShown;
    bool key[ALLEGRO_KEY_MAX];
    float cameraX;
    float fuel;
    int coin;
    Car car;
    Track track;
    std::vector<FuelTank*> fuelTanks;
    std::vector<Coin*> coins;
    ALLEGRO_SAMPLE* music = nullptr;
    ALLEGRO_SAMPLE_INSTANCE* backgroundMusic = nullptr;
    ALLEGRO_SAMPLE* gameover = nullptr;; // Add this to class members



    void initialize();
    void handleEvents();
    void update();
    void render();
    void cleanUp();
    int score = 0;

public:
    Game();
    void addFuel(float amount);
    void addCoin(int amount);
    ~Game();
    void run();
    ALLEGRO_SAMPLE* collectSound;

};
#endif 
