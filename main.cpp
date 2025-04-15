#include "function.h"
#include "gameobject.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "utils.h"
#include "constants.h"
#include "highscore.h"  // Bao gồm tệp header highscore.h


//game ne

const char* PLAYER_IMAGE = "D:/SDLgame/src/assets/player.png";
const char* ENEMY_IMAGE = "D:/SDLgame/src/assets/enemy.png";
const char* BULLET_IMAGE = "D:/SDLgame/src/assets/bullet.png";
const char* BACKGROUND_IMAGE = "D:/SDLgame/src/assets/background.jpg";
const char* SHOOT_SOUND = "D:/SDLgame/src/assets/shoot.mp3";
const char* HIT_SOUND = "D:/SDLgame/src/assets/hit.mp3";
const char* BACKGROUND_MUSIC = "D:/SDLgame/src/assets/music.mp3";
const char* FONT_PATH = "D:/SDLgame/src/assets/arial.ttf";

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* background = loadTexture(BACKGROUND_IMAGE, renderer);
    SDL_Texture* playerTex = loadTexture(PLAYER_IMAGE, renderer);
    SDL_Texture* enemyTex = loadTexture(ENEMY_IMAGE, renderer);
    SDL_Texture* bulletTex = loadTexture(BULLET_IMAGE, renderer);

    Mix_Chunk* shootSound = Mix_LoadWAV(SHOOT_SOUND);
    Mix_Chunk* hitSound = Mix_LoadWAV(HIT_SOUND);
    Mix_Music* music = Mix_LoadMUS(BACKGROUND_MUSIC);

    TTF_Font* font = TTF_OpenFont(FONT_PATH, 24);
    if (!font) {
        std::cerr << "❌ TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        return -1;
    }

    bool quitGame = false;

    // Màn hình chào khi bắt đầu game
    bool showStartScreen = true;
    while (showStartScreen) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // Hiển thị chữ "Press Enter to Start"
        SDL_Color white = { 255, 255, 255 };
        SDL_Surface* startSurface = TTF_RenderText_Solid(font, "Press Enter to Start", white);
        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
        SDL_Rect startRect = {
            (SCREEN_WIDTH - startSurface->w) / 2,
            (SCREEN_HEIGHT - startSurface->h) / 2,
            startSurface->w,
            startSurface->h
        };
        SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

        SDL_RenderPresent(renderer);

        SDL_FreeSurface(startSurface);
        SDL_DestroyTexture(startTexture);

        // Kiểm tra sự kiện nhấn phím Enter để bắt đầu game
        SDL_Event startEvent;
        while (SDL_PollEvent(&startEvent)) {
            if (startEvent.type == SDL_QUIT) {
                showStartScreen = false;
                quitGame = true;
            }
            if (startEvent.type == SDL_KEYDOWN && startEvent.key.keysym.sym == SDLK_RETURN) {
                showStartScreen = false;  // Chuyển đến màn hình chính của game
            }
        }

        SDL_Delay(100); // Giảm tải CPU
    }

    // Đọc điểm cao khi bắt đầu game
    int highScore = readHighScore();  // Đọc điểm cao từ file

    // 🔁 Vòng lặp restart game
    do {
        Player player(playerTex);
        vector<Enemy> enemies;
        vector<Bullet> bullets;
        Uint32 lastSpawn = SDL_GetTicks();
        int score = 0;
        int ammo = 10;
        bool canShoot = true;
        bool reloading = false;
        Uint32 reloadStartTime = 0;
        const int RELOAD_TIME = 2000;
        bool running = true;

        Mix_PlayMusic(music, -1);

        SDL_Event e;
        while (running) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    running = false;
                    quitGame = true;
                }

                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && ammo > 0 && canShoot) {
                    float centerX = player.x + PLAYER_WIDTH / 2;
                    float centerY = player.y + PLAYER_HEIGHT / 2;
                    float angleRad = atan2(mouseY - centerY, mouseX - centerX);
                    float spawnX = centerX + cos(angleRad) * (PLAYER_HEIGHT / 2);
                    float spawnY = centerY + sin(angleRad) * (PLAYER_HEIGHT / 2);

                    bullets.emplace_back(bulletTex, spawnX, spawnY, mouseX, mouseY);
                    Mix_PlayChannel(-1, shootSound, 0);
                    ammo--;
                    if (ammo == 0) canShoot = false;
                }

                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r && !reloading && ammo < 10) {
                    reloading = true;
                    reloadStartTime = SDL_GetTicks();
                    canShoot = false;
                }
            }

            const Uint8* keys = SDL_GetKeyboardState(NULL);
            player.move(keys);

            if (reloading && SDL_GetTicks() - reloadStartTime >= RELOAD_TIME) {
                ammo = 10;
                canShoot = true;
                reloading = false;
            }

            for (auto& bullet : bullets) bullet.update();
            bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return b.offScreen(); }), bullets.end());

            for (auto& enemy : enemies) enemy.moveToward(player);

            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (checkCollision(&player, &*it)) {
                    player.hp -= 10;
                    if (player.hp <= 0) {
                        // Game Over
                        if (score > highScore) {
                            highScore = score;  // Cập nhật điểm cao
                            writeHighScore(highScore);  // Ghi điểm cao vào file
                        }

                        // Hiển thị Game Over
                        SDL_Color white = { 255, 255, 255 };
                        SDL_Surface* overSurface = TTF_RenderText_Solid(font, "GAME OVER", white);
                        SDL_Texture* overTexture = SDL_CreateTextureFromSurface(renderer, overSurface);
                        SDL_Rect overRect = {
                            SCREEN_WIDTH / 2 - overSurface->w / 2,
                            SCREEN_HEIGHT / 2 - overSurface->h / 2 - 30,  // Dịch lên chút để không bị che
                            overSurface->w,
                            overSurface->h
                        };

                        // Hiển thị Click để Restart
                        SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Restart", white);
                        SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
                        SDL_Rect restartRect = {
                            SCREEN_WIDTH / 2 - restartSurface->w / 2,
                            SCREEN_HEIGHT / 2 + 10,  // Vị trí dưới Game Over
                            restartSurface->w,
                            restartSurface->h
                        };

                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, NULL);
                        SDL_RenderCopy(renderer, overTexture, NULL, &overRect);
                        SDL_RenderCopy(renderer, restartTexture, NULL, &restartRect);

                        // Hiển thị điểm cao nhất
                        string highScoreText = "High Score: " + to_string(highScore);
                        SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), white);
                        SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
                        SDL_Rect highScoreRect = {
                            SCREEN_WIDTH / 2 - highScoreSurface->w / 2,
                            SCREEN_HEIGHT / 2 + 50,  // Vị trí dưới "Restart"
                            highScoreSurface->w,
                            highScoreSurface->h
                        };
                        SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);
                        SDL_FreeSurface(highScoreSurface);
                        SDL_DestroyTexture(highScoreTexture);

                        SDL_RenderPresent(renderer);

                        SDL_FreeSurface(overSurface);
                        SDL_DestroyTexture(overTexture);
                        SDL_FreeSurface(restartSurface);
                        SDL_DestroyTexture(restartTexture);

                        bool waiting = true;
                        while (waiting) {
                            SDL_Event ev;
                            while (SDL_PollEvent(&ev)) {
                                if (ev.type == SDL_QUIT) {
                                    waiting = false;
                                }
                                if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                                    // Restart game bằng cách gọi lại main
                                    SDL_DestroyRenderer(renderer);
                                    SDL_DestroyWindow(window);
                                    Mix_CloseAudio();
                                    TTF_Quit();
                                    IMG_Quit();
                                    SDL_Quit();
                                    main(argc, argv); // ⚠️ Restart lại game
                                    return 0;
                                }
                            }
                            SDL_Delay(16); // Giảm CPU usage
                        }
                    }

                    it = enemies.erase(it);
                    Mix_PlayChannel(-1, hitSound, 0);
                    continue;
                }
                ++it;
            }

            for (auto it = bullets.begin(); it != bullets.end();) {
                bool hit = false;
                for (auto et = enemies.begin(); et != enemies.end();) {
                    if (checkCollision(&*it, &*et)) {
                        Mix_PlayChannel(-1, hitSound, 0);
                        score += 10;
                        et = enemies.erase(et);
                        it = bullets.erase(it);
                        hit = true;
                        break;
                    }
                    else ++et;
                }
                if (!hit) ++it;
            }

            if (SDL_GetTicks() - lastSpawn > 1000) {
                enemies.emplace_back(enemyTex, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
                lastSpawn = SDL_GetTicks();
            }

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, NULL);
            player.render(renderer, mouseX, mouseY);
            for (auto& enemy : enemies) enemy.render(renderer);
            for (auto& bullet : bullets) bullet.render(renderer);

            SDL_Color white = { 255, 255, 255 };
            string status = "HP: " + to_string(player.hp) + "  Score: " + to_string(score);

            SDL_Surface* textSurface = TTF_RenderText_Solid(font, status.c_str(), white);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { 10, 10, textSurface->w, textSurface->h };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            string ammoStatus = reloading ? "Reloading..." : "Ammo: " + to_string(ammo);
            SDL_Surface* ammoSurface = TTF_RenderText_Solid(font, ammoStatus.c_str(), white);
            SDL_Texture* ammoTexture = SDL_CreateTextureFromSurface(renderer, ammoSurface);
            SDL_Rect ammoRect = {
                SCREEN_WIDTH - ammoSurface->w - 20,
                SCREEN_HEIGHT - ammoSurface->h - 20,
                ammoSurface->w,
                ammoSurface->h
            };
            SDL_RenderCopy(renderer, ammoTexture, NULL, &ammoRect);
            SDL_FreeSurface(ammoSurface);
            SDL_DestroyTexture(ammoTexture);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    } while (!quitGame);

    // CLEANUP
    TTF_CloseFont(font);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(hitSound);
    Mix_FreeMusic(music);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(enemyTex);
    SDL_DestroyTexture(bulletTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
