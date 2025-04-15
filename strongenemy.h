#pragma once
#include "gameobject.h"
#include "player.h"

class StrongEnemy : public GameObject {
public:
    int hp;

    StrongEnemy(SDL_Texture* tex, float x, float y);
    void moveToward(const Player& player);
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect() const;
};
