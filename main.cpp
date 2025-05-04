#include <iostream>
#include "Hill Climb Racing/Game.h"
#include "Hill Climb Racing/Resources.h"


void must_init(bool test, const char* description) {
    if (test) return;
    std::cerr << "Couldn't initialize " << description << std::endl;
    exit(1);
}

int main() {
    Game game;
    game.run();
    return 0;
}
