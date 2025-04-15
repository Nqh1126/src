#pragma once
#include "gameobject.h"
#include "player.h"

class Enemy : public GameObject {
public:
    int hp;
    Enemy(SDL_Texture* tex, float px, float py);
    void moveToward(Player& player);
};
