#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

void must_init(bool test, const char* description) {
    if (test) return;
    std::cerr << "Couldn't initialize " << description << std::endl;
    exit(1);
}

template<typename T>
const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
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
    float angle = 0.0f;
    float angularVelocity = 0.0f;

    void jump() {
        if (onGround) {
            velocityY = -15;
            angularVelocity = -4.0f; // Reduced initial tilt
            onGround = false;
        }
    }

    void applyGravity() {
        velocityY += 1;
        y += velocityY;

        if (!onGround) {
            float angularAcceleration = 0.2f * sin(angle * ALLEGRO_PI / 180.0f); // Less sensitive
            angularVelocity += angularAcceleration;
            angle += angularVelocity;
            angularVelocity *= 0.93f;
        }
    }

    void applyFriction() {
        if (velocityX > 0) {
            velocityX -= 0.030;
            if (velocityX < 0) velocityX = 0;
        }
    }

    void accelerate() {
        velocityX += 0.5;
        velocityX = clamp(velocityX, 0.0f, 10.0f);
    }

    void checkTrackCollision(const std::vector<TrackSegment>& track, float carX) {
        onGround = false;

        for (const auto& segment : track) {
            if (carX >= segment.x1 && carX <= segment.x2) {
                float t = (carX - segment.x1) / (segment.x2 - segment.x1);
                float trackY = segment.y1 + t * (segment.y2 - segment.y1);
                float slope = (segment.y2 - segment.y1) / (segment.x2 - segment.x1);

                if (y + height >= trackY) {
                    y = trackY - height;
                    velocityY = 0;
                    onGround = true;

                    angle = atan(slope) * (180.0f / ALLEGRO_PI);
                    angularVelocity = 0;

                    velocityX += slope * 0.5;
                    velocityX = clamp(velocityX, -10.0f, 10.0f);
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
    must_init(al_init_font_addon(), "Font");
    must_init(al_init_native_dialog_addon(), "Native Dialog");

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(display, "display");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "event queue");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_BITMAP* sky = al_load_bitmap("sky.jpg");
    must_init(sky, "sky background");

    ALLEGRO_BITMAP* car_image = al_load_bitmap("car_image.png");
    must_init(car_image, "car image");

    bool running = true;
    bool gameOverShown = false;
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
                if (fuel > 0 && !gameOverShown) {
                    if (key[ALLEGRO_KEY_RIGHT] || key[ALLEGRO_KEY_D]) {
                        car.accelerate();
                    }
                    else {
                        car.applyFriction();
                    }

                    if (key[ALLEGRO_KEY_SPACE]) car.jump();

                    if (!car.onGround) {
                        if (key[ALLEGRO_KEY_A]) car.angularVelocity -= 0.7f;
                        if (key[ALLEGRO_KEY_D]) car.angularVelocity += 0.7f;
                    }

                    cameraX += car.velocityX;
                    cameraX = std::max(0.0f, cameraX);

                    float carWorldX = cameraX + CAR_SCREEN_X;
                    car.applyGravity();
                    car.checkTrackCollision(track, carWorldX);

                    if (std::abs(car.angle) > 90.0f && !gameOverShown) {
                        gameOverShown = true;
                        al_show_native_message_box(display, "Game Over", "Car Flipped!",
                            "You crashed by flipping over!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
                    }

                    fuel -= 0.095f;
                    fuel = std::max(0.0f, fuel);
                }

                // Drawing
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_scaled_bitmap(sky, 0, 0, al_get_bitmap_width(sky), al_get_bitmap_height(sky),
                    0, 0, SCREEN_W, SCREEN_H, 0);

                for (const auto& segment : track) {
                    al_draw_line(segment.x1 - cameraX, segment.y1, segment.x2 - cameraX, segment.y2,
                        al_map_rgb(0, 0, 0), 8);
                }

                float cx = al_get_bitmap_width(car_image) / 2.0f;
                float cy = al_get_bitmap_height(car_image) / 2.0f;
                float scale_x = car.width / al_get_bitmap_width(car_image);
                float scale_y = car.height / al_get_bitmap_height(car_image);
                al_draw_scaled_rotated_bitmap(car_image, cx, cy,
                    CAR_SCREEN_X + car.width / 2, car.y + car.height / 2,
                    scale_x, scale_y,
                    car.angle * ALLEGRO_PI / 180.0f, 0);

                // Fuel bar
                al_draw_filled_rectangle(20, 20, 20 + (fuel * 3), 40, al_map_rgb(255, 0, 0));
                al_draw_rectangle(20, 20, 20 + 300, 40, al_map_rgb(255, 255, 255), 2);

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

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(sky);
    al_destroy_bitmap(car_image);

    return 0;
}
