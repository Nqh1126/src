#include "function.h"
#include "player.h"
#include "constants.h"

Player::Player(SDL_Texture* tex)
    : GameObject(tex, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, PLAYER_WIDTH, PLAYER_HEIGHT), hp(100) {
}

void Player::move(const Uint8* keys) {
    if (keys[SDL_SCANCODE_W]) y -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_S]) y += PLAYER_SPEED;
    if (keys[SDL_SCANCODE_A]) x -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_D]) x += PLAYER_SPEED;

    if (x < 0) x = 0;
    if (x > SCREEN_WIDTH - PLAYER_WIDTH) x = SCREEN_WIDTH - PLAYER_WIDTH;
    if (y < 0) y = 0;
    if (y > SCREEN_HEIGHT - PLAYER_HEIGHT) y = SCREEN_HEIGHT - PLAYER_HEIGHT;
}

void Player::render(SDL_Renderer* renderer, int mouseX, int mouseY) {
    SDL_Rect dstRect = { static_cast<int>(x), static_cast<int>(y), rect.w, rect.h };

    // Tính góc từ tâm người chơi đến chuột
    float centerX = x + rect.w / 2;
    float centerY = y + rect.h / 2;
    float angle = atan2(mouseY - centerY, mouseX - centerX) * 180 / M_PI;

    SDL_Point center = { rect.w / 2, rect.h / 2 };
    SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, angle, &center, SDL_FLIP_NONE);
}
