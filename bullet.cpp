#include "function.h"
#include "bullet.h"
#include "constants.h"

// Constructor: spawn tại tâm mũi tàu → phải trừ nửa kích thước
Bullet::Bullet(SDL_Texture* tex, float sx, float sy, float tx, float ty)
    : GameObject(tex, sx - BULLET_WIDTH / 2, sy - BULLET_HEIGHT / 2, BULLET_WIDTH, BULLET_HEIGHT) {

    float len = sqrt((tx - sx) * (tx - sx) + (ty - sy) * (ty - sy));
    dx = (tx - sx) / len * BULLET_SPEED;
    dy = (ty - sy) / len * BULLET_SPEED;

    angle = atan2(ty - sy, tx - sx) * 180 / M_PI;
}

void Bullet::update() {
    x += dx;
    y += dy;
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect dstRect = { static_cast<int>(x), static_cast<int>(y), rect.w, rect.h };
    SDL_Point center = { rect.w / 2, rect.h / 2 };
    SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, angle, &center, SDL_FLIP_NONE);
}

bool Bullet::offScreen() {
    return x < -rect.w || x > SCREEN_WIDTH + rect.w ||
        y < -rect.h || y > SCREEN_HEIGHT + rect.h;
}

// Hàm trả về hitbox chính xác cho va chạm
SDL_Rect Bullet::getRect() const {
    return SDL_Rect{
        static_cast<int>(x),
        static_cast<int>(y),
        rect.w,
        rect.h
    };
}
