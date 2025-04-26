#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

void must_init(bool test, const char* description) {
    if (test) return;
    std::cerr << "Couldn't initialize " << description << std::endl;
    exit(1);
}

const int SCREEN_W = 1000;
const int SCREEN_H = 700;
const float CAR_SCREEN_X = 120;

const int widthCar = 120;
const int heightCar = 70;

struct TrackSegment {
    float x1, y1, x2, y2;
};

std::vector<TrackSegment> generateTrack() {
    std::vector<TrackSegment> track;
    float prevY = SCREEN_H * 3 / 4;

    for (int x = 0; x < SCREEN_W * 10; x += 50) {
        float y = (x < CAR_SCREEN_X ? prevY : prevY + (rand() % 40 - 20));
        y = std::max(100.0f, std::min((float)SCREEN_H - 100, y));
        if (x > 0)
            track.push_back({ (float)x - 50, prevY, (float)x, y });
        prevY = y;
    }
    return track;
}

struct Car {
    float y;
    float width = widthCar, height = heightCar;
    float velocityY = 0;
    float velocityX = 0;
    bool onGround = false;

    void jump() {
        if (onGround) {
            velocityY = -15;
            onGround = false;
        }
    }
    void applyGravity() {
        velocityY += 1;
        y += velocityY;
    }

    void applyFriction() {
        if (velocityX > 0) {
            velocityX -= 0.030;  // friction slows you down
            if (velocityX < 0) velocityX = 0; // stop fully
        }
    }

    void accelerate() {
        velocityX += 0.5; // acceleration when gas is pressed
        if (velocityX > 10) velocityX = 10; // max speed limit
    }

    void checkTrackCollision(const std::vector<TrackSegment>& track, float carX) {
        onGround = false; 

        for (const auto& segment : track) {
            if (carX >= segment.x1 && carX <= segment.x2) {
                float t = (carX - segment.x1) / (segment.x2 - segment.x1);
                float trackY = segment.y1 + t * (segment.y2 - segment.y1);

                float slope = (segment.y2 - segment.y1) / (segment.x2 - segment.x1); // Rise over run

                if (y + height >= trackY) {
                    y = trackY - height;
                    velocityY = 0;
                    onGround = true;

                    //horizontal sliding
                    velocityX += slope * 0.5; 

                    if (velocityX > 10) velocityX = 10;
                    if (velocityX < -10) velocityX = -10;

                    return;
                }
            }
        }
    }

};

int main() {
    srand(time(0));

    must_init(al_init(), "Allegro");
    must_init(al_init_primitives_addon(), "Primitives");
    must_init(al_install_keyboard(), "Keyboard");
    must_init(al_init_image_addon(), "Image");

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(display, "display");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "event queue");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_BITMAP* sky = al_load_bitmap("sky.png");
    must_init(sky, "sky background");

    ALLEGRO_BITMAP* car_image = al_load_bitmap("car_image.png");
    must_init(car_image, "car image");

    bool running = true;
    bool key[ALLEGRO_KEY_MAX] = { false };
    ALLEGRO_EVENT event;

    float cameraX = 0;
    Car car = { SCREEN_H / 2 };
    std::vector<TrackSegment> track = generateTrack();

    float fuel = 100.0f;

    al_start_timer(timer);

    while (running) {
        while (!al_is_event_queue_empty(queue)) {
            al_wait_for_event(queue, &event);

            if (event.type == ALLEGRO_EVENT_TIMER) {
                // Movement
                if (fuel > 0) {
                    if (key[ALLEGRO_KEY_RIGHT] || key[ALLEGRO_KEY_D]) {
                        car.accelerate();
                    }
                    else {
                        car.applyFriction();
                    }

                    //if (key[ALLEGRO_KEY_LEFT] || key[ALLEGRO_KEY_A]) {  //moving left. since we wont move left i left it out
                    //    cameraX -= 5;  
                    //}

                    cameraX += car.velocityX;

                    cameraX = std::max(0.0f, cameraX);
                    //if (key[ALLEGRO_KEY_SPACE] || key[ALLEGRO_KEY_UP]) car.jump(); ->since the cars dont jump?
                }
                float carWorldX = cameraX + CAR_SCREEN_X;
                car.applyGravity();
                car.checkTrackCollision(track, carWorldX);

                // fuel slowwly decreases
                fuel -= 0.095f;
                if (fuel <= 0) {
                    fuel = 0;
                }

                // Drawing
                al_clear_to_color(al_map_rgb(0, 0, 0));

                al_draw_scaled_bitmap(sky, 0, 0, al_get_bitmap_width(sky), al_get_bitmap_height(sky),
                    0, 0, SCREEN_W, SCREEN_H, 0);

                for (const auto& segment : track) {
                    al_draw_line(segment.x1 - cameraX, segment.y1, segment.x2 - cameraX, segment.y2,
                        al_map_rgb(0, 0, 0), 8);
                }

                al_draw_scaled_bitmap(car_image, 0, 0, al_get_bitmap_width(car_image), al_get_bitmap_height(car_image),
                    CAR_SCREEN_X, car.y, car.width, car.height, 0);

                // Fuel Bar
                al_draw_filled_rectangle(20, 20, 20 + (fuel * 3), 40, al_map_rgb(255, 0, 0)); // red bar
                al_draw_rectangle(20, 20, 20 + (100 * 3), 40, al_map_rgb(255, 255, 255), 2);   // white outline

                al_flip_display();
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
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

    al_destroy_display(display);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(sky);
    al_destroy_bitmap(car_image);

    return 0;
}
