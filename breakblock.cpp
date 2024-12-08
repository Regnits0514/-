#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "game.h"
#define _CRT_SECURE_NO_WARNINGS

#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

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

// SQLite 데이터베이스 연결과 저장을 위한 함수
void save_score_to_db(const char* player_name, int score) {
    sqlite3* db = nullptr;
    char* err_msg = nullptr;

    int rc = sqlite3_open("breakblock.db", &db);

    if (rc != SQLITE_OK) {
        printf("SQLite open error: %s\n", sqlite3_errmsg(db));
        return;
    }

    // SQL 쿼리 문자열 준비
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO scores (name, score) VALUES ('%s', %d);", player_name, score);

    // SQL 쿼리 실행
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else {
        printf("Score saved successfully!\n");
    }

    // 데이터베이스 연결 닫기
    sqlite3_close(db);
}

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
        lives--;  // 목숨 감소
        if (lives > 0) {
            // 공과 패들 초기화
            ball.x = SCREEN_WIDTH / 2 - BALL_RADIUS;
            paddle.x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
            ball.y = SCREEN_HEIGHT - 100 - BALL_RADIUS;  // 공 초기 위치
        }
        else {
            // 게임 종료

            printf("Game Over! Final Score: %d\n", score);

            SDL_Delay(2000);  // 2초간 점수 표시 후 게임 종료

            TTF_Quit();
            SDL_Quit();

            // 이름 입력 받기
            char player_name[50];
            printf("Enter your name: ");
            fgets(player_name, sizeof(player_name), stdin);

            // 줄바꿈 제거
            player_name[strcspn(player_name, "\n")] = 0;

            // 점수와 이름을 DB에 저장
            save_score_to_db(player_name, score);

        }
    }
}

void init_breakout_game() {
    std::cout << "Breakout Game Initialized.\n";
    // 초기화 코드
}



// 메인 함수
void run_breakout_game() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Breakout Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);

    init_game();

    SDL_Event e;
    bool quit = false;

    while (!quit && lives > 0) {  // lives가 0보다 큰 동안만 게임 진행
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
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

    // 게임 오버 처리
    if (lives <= 0) {
        printf("Game Over! Final Score: %d\n", score);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}