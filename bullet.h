#pragma once
#include "gameobject.h"

class Bullet : public GameObject {
public:
    float dx, dy;
    float angle;

    Bullet(SDL_Texture* tex, float sx, float sy, float tx, float ty);
    void update();
    void render(SDL_Renderer* renderer);
    bool offScreen();
    SDL_Rect getRect() const override; // Thêm override cho đúng
};

