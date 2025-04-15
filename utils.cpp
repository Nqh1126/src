#include "utils.h"

bool checkCollision(GameObject* a, GameObject* b) {
    SDL_Rect ra = a->getRect();
    SDL_Rect rb = b->getRect();
    return SDL_HasIntersection(&ra, &rb);
}
