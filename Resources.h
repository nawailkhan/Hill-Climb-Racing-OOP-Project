#ifndef RESOURCES_H
#define RESOURCES_H

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
#include <algorithm>


const int SCREEN_W = 1000;
const int SCREEN_H = 700;
const float CAR_SCREEN_X = 120;
const int WIDTH_CAR = 120;
const int HEIGHT_CAR = 70;

template<typename T>
inline const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}

void must_init(bool test, const char* description);

#endif 
