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
    std::vector<float> controlPoints;
    const int numPoints = 200;
    const int segmentLength = 50;

    float prevY = SCREEN_H * 3 / 4;
    controlPoints.push_back(prevY);

    // Generate control points with controlled randomness
    for (int i = 1; i < numPoints; ++i) {
        float offset = (rand() % 60 - 30);
        float newY = clamp(controlPoints.back() + offset, 100.0f, (float)SCREEN_H - 100);
        controlPoints.push_back(newY);
    }

    for (int i = 0; i < (int)controlPoints.size() - 1; ++i) {
        float x_start = i * segmentLength;
        float y1 = controlPoints[i];
        float y2 = controlPoints[i + 1];

        for (int j = 0; j < segmentLength; ++j) {
            float t = (float)j / segmentLength;
            float t2 = (1 - cos(t * ALLEGRO_PI)) / 2; // cosine interpolation
            float y = y1 * (1 - t2) + y2 * t2;

            float x1 = x_start + j;
            float x2 = x_start + j + 1;

            float next_t = (float)(j + 1) / segmentLength;
            float next_t2 = (1 - cos(next_t * ALLEGRO_PI)) / 2;
            float y_next = y1 * (1 - next_t2) + y2 * next_t2;

            track.push_back({ x1, y, x2, y_next });
        }
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
        velocityX += 0.35;
        velocityX = clamp(velocityX, 0.0f, 8.0f);
    }

    void checkTrackCollision(const std::vector<TrackSegment>& track, float carX) {
        onGround = false;

        float rearX = carX;
        float frontX = carX + width;

        float rearY = y + height;
        float frontY = y + height;

        auto getTrackY = [&](float x) -> float {
            for (const auto& segment : track) {
                if (x >= segment.x1 && x <= segment.x2) {
                    float t = (x - segment.x1) / (segment.x2 - segment.x1);
                    return segment.y1 + t * (segment.y2 - segment.y1);
                }
            }
            return SCREEN_H; // Fallback (off screen)
            };

        float trackRearY = getTrackY(rearX);
        float trackFrontY = getTrackY(frontX);

        float dx = frontX - rearX;
        float dy = trackFrontY - trackRearY;

        float desiredAngle = atan2(dy, dx) * (180.0f / ALLEGRO_PI);
        float avgY = (trackRearY + trackFrontY) / 2.0f;

        y = avgY - height;
        angle = desiredAngle;
        angularVelocity = 0;
        velocityY = 0;
        onGround = true;
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
