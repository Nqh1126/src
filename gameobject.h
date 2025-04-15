#pragma once
#include "function.h"
#include "constants.h"

class GameObject {
public:
    float x, y;
    SDL_Texture* texture;
    SDL_Rect rect;

    GameObject(SDL_Texture* tex, float px, float py, int w, int h)
        : texture(tex), x(px), y(py) {
        rect = { static_cast<int>(x), static_cast<int>(y), w, h };
    }

    virtual void render(SDL_Renderer* renderer) {
        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    virtual SDL_Rect getRect() const {
        return SDL_Rect{
            static_cast<int>(x),
            static_cast<int>(y),
            rect.w,
            rect.h
        };
    }
};
