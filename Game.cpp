#include "Game.h"
#include "Physics.h"
#include "Track.h"
#include "Car.h"
#include "Resources.h"
#include "Collectibles.h"
#include <iostream>

Game::Game() :
    display(nullptr),
    queue(nullptr),
    timer(nullptr),
    font(nullptr),
    sky(nullptr),
    car_image(nullptr),
    running(true),
    gameOverShown(false),
    cameraX(0),
    fuel(100.0f),

    car(SCREEN_H / 2) {

    for (bool& k : key) k = false;
    initialize();
    for (int i = 0; i < 10; ++i) {  
        fuelTanks.push_back(SpawnFuelTank(track));
    } 
}

Game::~Game() {
    cleanUp();
}

void Game::initialize() {
    srand(time(0));

    must_init(al_init(), "Allegro");
    must_init(al_init_primitives_addon(), "Primitives");
    must_init(al_install_keyboard(), "Keyboard");
    must_init(al_init_image_addon(), "Image");
    must_init(al_init_font_addon(), "Font");
    must_init(al_init_native_dialog_addon(), "Native Dialog");
    must_init(al_init_ttf_addon(), "ttf addon");

    font = al_create_builtin_font();
	must_init(font, "font");

    display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(display, "display");

    queue = al_create_event_queue();
    must_init(queue, "event queue");

    timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    font = al_create_builtin_font();
    font = al_load_ttf_font("arial.ttf", 32, 0); 
    must_init(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
   
    fuelImage = al_load_bitmap("fueltank.png");
	must_init(fuelImage, "fuel image");

    sky = al_load_bitmap("sky.png");
    must_init(sky, "sky background");

    car_image = al_load_bitmap("car_image.png");
    must_init(car_image, "car image");

    al_start_timer(timer);
}

void Game::handleEvents() {
    ALLEGRO_EVENT event;
    while (!al_is_event_queue_empty(queue)) {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            key[event.keyboard.keycode] = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            key[event.keyboard.keycode] = false;
        }
    }
}

void Game::update() {
    if (fuel > 0 && !gameOverShown) {
        if (key[ALLEGRO_KEY_RIGHT] || key[ALLEGRO_KEY_D]) {
            car.accelerate();
        }
        else {
            car.applyFriction();
        }

        if (key[ALLEGRO_KEY_SPACE]) {
            car.jump();
        }

        if (!car.isOnGround()) {
            if (key[ALLEGRO_KEY_A]) car.rotate(-1.0f);
            if (key[ALLEGRO_KEY_D]) car.rotate(1.0f);
        }

        cameraX += car.getVelocityX();
        cameraX = std::max(0.0f, cameraX);

        float carWorldX = cameraX + CAR_SCREEN_X;
        car.update(track, carWorldX);

        if (Physics::checkFlipCondition(car.getAngle()) && !gameOverShown) {
            gameOverShown = true;
            al_show_native_message_box(display, "Game Over", "Car Flipped!",
                "You crashed by flipping over!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        }

        float carX = cameraX + CAR_SCREEN_X;

        for (FuelTank* tank : fuelTanks) {
            if (!tank->isCollected()) {
                if (carX + WIDTH_CAR > tank->getX() &&
                    carX < tank->getX() + tank->getWidth() &&
                    car.getY() + HEIGHT_CAR > tank->getY() &&
                    car.getY() < tank->getY() + tank->getHeight()) {
                    tank->applyEffect(*this);
                }
            }
        }
        score = static_cast<int>(cameraX / 10.0f); // Adjust scale: every 10 pixels = 1 "meter"

        //fuel update
        fuel -= 0.095f;
        fuel = std::max(0.0f, fuel);
    }
}

void Game::render() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(sky, 0, 0, al_get_bitmap_width(sky), al_get_bitmap_height(sky),
        0, 0, SCREEN_W, SCREEN_H, 0);

    // track
    for (const auto& segment : track.getSegments()) {
        al_draw_line(segment.x1 - cameraX, segment.y1, segment.x2 - cameraX, segment.y2,
            al_map_rgb(0, 0, 0), 8);
    }

    // car
    if (!car_image) {
        al_draw_filled_rectangle(
            CAR_SCREEN_X, car.getY(),
            CAR_SCREEN_X + WIDTH_CAR, car.getY() + HEIGHT_CAR,
            al_map_rgb(255, 0, 0));
        return;
    }

    for (FuelTank* tank : fuelTanks) {
        tank->render(cameraX);
    }

    al_draw_filled_rectangle(20, 20, 20 + (fuel * 3), 40, al_map_rgb(255, 0, 0));

    float img_w = al_get_bitmap_width(car_image);
    float img_h = al_get_bitmap_height(car_image);
    float scale_x = WIDTH_CAR / img_w;
    float scale_y = HEIGHT_CAR / img_h;
    float center_x = CAR_SCREEN_X + WIDTH_CAR / 2;
    float center_y = car.getY() + HEIGHT_CAR / 2;
    float angle_rad = car.getAngle() * ALLEGRO_PI / 180.0f;

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); // added these 2 bec it wasnt working w/o
    // this prevented transparency otherwise the car wasnt visible idk why
    al_draw_circle(center_x, center_y, 10, al_map_rgb(0, 255, 0), 2);
    // this draws green circle at car's centre whch makes it easier to identify the problem
    // the car was being initialozed it was just not visible so the set bender command did the work
    al_draw_scaled_rotated_bitmap(
        car_image,
        img_w / 2, img_h / 2,    // center of bitmap
        center_x, center_y,   // pos on screen
        scale_x, scale_y,     // scale factors
        angle_rad,            // rotation in radians
        0);

	al_draw_filled_rectangle(20, 20, 20 + (fuel * 3), 40, al_map_rgb(255, 0, 0));
	al_draw_rectangle(20, 20, 20 + 300, 40, al_map_rgb(255, 255, 255), 2);
	
	al_draw_textf(font, al_map_rgb(255, 255, 255), 20, 50, 0, "Fuel: %.1f%%", fuel);
	
	char scoreText[64];
	snprintf(scoreText, sizeof(scoreText), "Distance: %d m", score);
	al_draw_filled_rectangle(SCREEN_W - 280,5, SCREEN_W - 10, 60, al_map_rgba(0, 0, 0, 180));
	al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W - 250, 15, 0, scoreText);
	
	
	al_flip_display();
}

void Game::cleanUp() {
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(sky);
    al_destroy_bitmap(car_image);
    for (FuelTank* tank : fuelTanks) {
        delete tank;
    }
    fuelTanks.clear();
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::addFuel(float amount) {
    fuel += amount;
    if (fuel > 100.0f) fuel = 100.0f;
}
