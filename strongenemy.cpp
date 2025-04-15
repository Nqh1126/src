#include "strongenemy.h"

StrongEnemy::StrongEnemy(SDL_Texture* tex, float startX, float startY)
    : Enemy(tex, startX, startY), hp(3) {
}

void StrongEnemy::moveToward(Player& player) {
    float dx = player.x - x;
    float dy = player.y - y;
    float len = sqrt(dx * dx + dy * dy);
    x += (dx / len) * (ENEMY_SPEED + 1); // nhanh hơn
    y += (dy / len) * (ENEMY_SPEED + 1);
}

SDL_Rect StrongEnemy::getRect() const {
    return { static_cast<int>(x), static_cast<int>(y), rect.w, rect.h };
}

void StrongEnemy::render(SDL_Renderer* renderer) {
    SDL_Rect dst = { static_cast<int>(x), static_cast<int>(y), rect.w, rect.h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    // Vẽ thanh máu
    SDL_Rect hpBar = { dst.x, dst.y - 6, rect.w, 4 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // viền đỏ
    SDL_RenderFillRect(renderer, &hpBar);
    hpBar.w = rect.w * hp / 3;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // phần còn lại màu xanh
    SDL_RenderFillRect(renderer, &hpBar);
}