#define SDL_MAIN_HANDLED
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "common.h"
#include "game.h"
#include <string.h>

#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

#define SW 1920
#define SH 1080
#define PADDLE_W 10
#define PADDLE_H 80
#define BALL_R 15   // 공의 반지름을 15로 크게 설정
#define SPEED 5

int p_score = 0, ai_score = 0, missed = 0;


void draw_circle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;  // X 좌표에서 중심점까지의 거리
            int dy = radius - h;  // Y 좌표에서 중심점까지의 거리
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void save_score_to_db(const char* player_name, int score, const char* result) {
    sqlite3* db = nullptr;
    char* err_msg = nullptr;

    // 데이터베이스 열기
    int rc = sqlite3_open("pingpong.db", &db);

    if (rc != SQLITE_OK) {
        printf("SQLite open error: %s\n", sqlite3_errmsg(db));
        return;
    }

    // SQL 쿼리 문자열 준비
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO scores (name, score, result) VALUES ('%s', %d, '%s');", player_name, score, result);

    // SQL 쿼리 실행
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);  // 에러 메시지 메모리 해제
    }
    else {
        printf("Score saved successfully!\n");
    }

    // 데이터베이스 연결 닫기
    sqlite3_close(db);
}


void run_pong_game() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("AI Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SW, SH, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);

    SDL_Rect p_paddle = { 10, SH / 2 - PADDLE_H / 2, PADDLE_W, PADDLE_H };
    SDL_Rect ai_paddle = { SW - 20, SH / 2 - PADDLE_H / 2, PADDLE_W, PADDLE_H };
    int ball_x = SW / 2, ball_y = SH / 2, ball_vel_x = -SPEED * 3, ball_vel_y = -SPEED * 3;

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) quit = 1;

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            p_paddle.y += (keys[SDL_SCANCODE_UP] ? -SPEED / 2 : (keys[SDL_SCANCODE_DOWN] ? SPEED / 2 : 0));
        }
        else
        {
            p_paddle.y += (keys[SDL_SCANCODE_UP] ? -SPEED * 2 : (keys[SDL_SCANCODE_DOWN] ? SPEED * 2 : 0));
        }
        if (p_paddle.y < 0) p_paddle.y = 0;
        if (p_paddle.y + PADDLE_H > SH) p_paddle.y = SH - PADDLE_H;

        ball_x += ball_vel_x;
        ball_y += ball_vel_y;
        if (ball_y - BALL_R <= 0 || ball_y + BALL_R >= SH) ball_vel_y = -ball_vel_y;

        if (ball_x - BALL_R <= p_paddle.x + PADDLE_W && ball_y >= p_paddle.y && ball_y <= p_paddle.y + PADDLE_H)
            ball_vel_x = -ball_vel_x;

        if (ball_x + BALL_R >= ai_paddle.x && ball_y >= ai_paddle.y && ball_y <= ai_paddle.y + PADDLE_H)
            ball_vel_x = -ball_vel_x;

        if (ball_x - BALL_R <= 0) { ai_score++; ball_x = SW / 2; ball_y = SH / 2; }
        if (ball_x + BALL_R >= SW) { p_score++; ball_x = SW / 2; ball_y = SH / 2; }

        if (ball_y > ai_paddle.y + PADDLE_H / 2) ai_paddle.y += SPEED;
        else if (ball_y < ai_paddle.y + PADDLE_H / 2) ai_paddle.y -= SPEED;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &p_paddle);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &ai_paddle);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_circle(renderer, ball_x, ball_y, BALL_R);  // 공을 둥글게 그리기

        char score_text[30];
        sprintf_s(score_text, sizeof(score_text), "P: %d AI: %d", p_score, ai_score);

        render_text(renderer, font, SW / 2 - 50, 20, score_text);

        SDL_RenderPresent(renderer);

        // 점수가 5점에 도달한 경우 게임 종료
        if (p_score >= 5 || ai_score >= 5) {
            quit = 1;
        }

        SDL_Delay(16);
    }

    // 게임 종료 후 처리
    char winner_text[50];
    const char* result = "";
    if (p_score >= 5) {
        sprintf_s(winner_text, sizeof(winner_text), "Player wins with %d points!", p_score);
        result = "win";  // 플레이어가 이긴 경우
    }
    else {
        sprintf_s(winner_text, sizeof(winner_text), "AI wins with %d points!", ai_score);
        result = "defeat";  // AI가 이긴 경우
    }

    printf("%s\n", winner_text); // 콘솔에 승자 출력

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    // 이름 입력 받기
    char player_name[50];
    printf("Enter your name: ");
    fgets(player_name, sizeof(player_name), stdin);

    // 줄바꿈 제거
    player_name[strcspn(player_name, "\n")] = 0;

    // 점수와 이름을 DB에 저장
    save_score_to_db(player_name, p_score, result);
}
