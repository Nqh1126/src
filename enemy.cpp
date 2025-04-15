#include "function.h"
#include "enemy.h"
#include "player.h"

Enemy::Enemy(SDL_Texture* tex, float px, float py)
    : GameObject(tex, px, py, PLAYER_WIDTH, PLAYER_HEIGHT), hp(20) {
}

void Enemy::moveToward(Player& player) {
    float dx = player.x - x;
    float dy = player.y - y;
    float len = sqrt(dx * dx + dy * dy);
    x += dx / len * ENEMY_SPEED;
    y += dy / len * ENEMY_SPEED;
}
