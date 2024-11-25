#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 10
#define BLOCK_WIDTH 60
#define BLOCK_HEIGHT 20
#define BLOCK_ROWS 30
#define BLOCK_COLS 29
#define PADDLE_SPEED 6
#define BALL_SPEED 4

// 전역 변수
int ballVelX = BALL_SPEED, ballVelY = -BALL_SPEED;
int score = 0;
int lives = 3;
SDL_Rect paddle;
SDL_Rect ball;
SDL_Rect blocks[BLOCK_ROWS][BLOCK_COLS];

// 게임 초기화
void init_game() {
    paddle = { SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIGHT - 20, PADDLE_WIDTH, PADDLE_HEIGHT };
    ball = { SCREEN_WIDTH / 2 - BALL_RADIUS, SCREEN_HEIGHT - 100 - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2 };


    // 블록 배열 초기화
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            blocks[i][j] = { j * (BLOCK_WIDTH + 5) + 30, i * (BLOCK_HEIGHT + 5) + 30, BLOCK_WIDTH, BLOCK_HEIGHT };
        }
    }
}

// 텍스트 렌더링 함수
void render_text(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const char* text) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, { 255, 255, 255, 255 });
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// 공, 패들, 블록 그리기
void draw_game(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 패들
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &paddle);

    // 공
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball);

    // 블록
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].w > 0) {
                SDL_RenderFillRect(renderer, &blocks[i][j]);
            }
        }
    }

    // 점수 및 생명 표시
    char scoreText[50];
    sprintf_s(scoreText, sizeof(scoreText), "Score: %d Lives: %d", score, lives);


    render_text(renderer, font, 10, 10, scoreText);

    SDL_RenderPresent(renderer);
}

// 공과 블록 충돌 체크
void check_collisions() {
    // 블록과의 충돌
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].w > 0) { // 블록이 활성화된 상태일 때만
                if (SDL_HasIntersection(&ball, &blocks[i][j])) {
                    blocks[i][j].w = 0; // 블록 제거
                    blocks[i][j].h = 0;
                    score += 10; // 점수 증가
                    ballVelY = -ballVelY; // 공 방향 반전
                }
            }
        }
    }

    // 벽 충돌
    if (ball.x <= 0 || ball.x + ball.w >= SCREEN_WIDTH) {
        ballVelX = -ballVelX;
    }
    if (ball.y <= 0) {
        ballVelY = -ballVelY;
    }

    // 패들과 충돌
    if (SDL_HasIntersection(&ball, &paddle)) {
        ballVelY = -ballVelY;
    }

    // 바닥에 닿았을 때
    if (ball.y + ball.h >= SCREEN_HEIGHT) {
        lives--;
        if (lives > 0) {
            ball.x = SCREEN_WIDTH / 2 - BALL_RADIUS;
            paddle.x = SCREEN_WIDTH / 2 - BALL_RADIUS;
            ball.y = SCREEN_HEIGHT - 100 - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2;
        }
        else {
            // 게임 종료
            printf("Game Over! Final Score: %d\n", score);
        }
    }
}

// 메인 함수
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Breakout Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);

    init_game();

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // 키 입력 처리 (좌우로 패들 이동)
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_LEFT]) {
            if (paddle.x > 0) {
                paddle.x -= PADDLE_SPEED;
            }
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
            if (paddle.x + paddle.w < SCREEN_WIDTH) {
                paddle.x += PADDLE_SPEED;
            }
        }

        // 공 이동
        ball.x += ballVelX;
        ball.y += ballVelY;

        // 충돌 처리
        check_collisions();

        // 게임 그리기
        draw_game(renderer, font);

        SDL_Delay(16); // 약간의 딜레이를 두어 FPS를 60으로 설정
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
