#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "game.h"

#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define CELL_SIZE 20
#define INITIAL_SNAKE_LENGTH 5
#define SNAKE_SPEED 25  // 밀리초 단위로 이동 주기

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[SCREEN_WIDTH * SCREEN_HEIGHT / (CELL_SIZE * CELL_SIZE)];
    int length;
    int dx, dy;  // 이동 방향
} Snake;

void init_snake(Snake* snake) {
    snake->length = INITIAL_SNAKE_LENGTH;
    snake->dx = CELL_SIZE;  // 오른쪽으로 이동 시작
    snake->dy = 0;
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = 100 - i * CELL_SIZE;
        snake->body[i].y = 100;
    }
}

void move_snake(Snake* snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;
}

int check_collision(Snake* snake, Point* food) {
    // 벽 충돌 검사
    if (snake->body[0].x < 0 || snake->body[0].x >= SCREEN_WIDTH || snake->body[0].y < 0 || snake->body[0].y >= SCREEN_HEIGHT)
        return 1;

    // 자기 몸과 충돌 검사
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            return 1;
    }

    // 음식 먹기
    if (snake->body[0].x == food->x && snake->body[0].y == food->y) {
        snake->length++;
        return 2;  // 음식 먹음
    }

    return 0;  // 충돌 없음
}

void generate_food(Point* food, Snake* snake) {
    int valid = 0;
    while (!valid) {
        valid = 1;
        food->x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
        food->y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
        for (int i = 0; i < snake->length; i++) {
            if (food->x == snake->body[i].x && food->y == snake->body[i].y) {
                valid = 0;  // 음식이 뱀의 몸 위에 생기지 않도록
                break;
            }
        }
    }
}

void handle_input(Snake* snake, SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_UP: if (snake->dy == 0) { snake->dx = 0; snake->dy = -CELL_SIZE; } break;
        case SDLK_DOWN: if (snake->dy == 0) { snake->dx = 0; snake->dy = CELL_SIZE; } break;
        case SDLK_LEFT: if (snake->dx == 0) { snake->dx = -CELL_SIZE; snake->dy = 0; } break;
        case SDLK_RIGHT: if (snake->dx == 0) { snake->dx = CELL_SIZE; snake->dy = 0; } break;
        }
    }
}

void render_game(SDL_Renderer* renderer, Snake* snake, Point* food) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 배경색 (검정)
    SDL_RenderClear(renderer);

    // 뱀 그리기
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // 초록색
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = { snake->body[i].x, snake->body[i].y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    // 음식 그리기
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // 빨간색
    SDL_Rect food_rect = { food->x, food->y, CELL_SIZE, CELL_SIZE };
    SDL_RenderFillRect(renderer, &food_rect);

    SDL_RenderPresent(renderer);
}

void init_snake_game() {
    std::cout << "Snake Game Initialized.\n";
    // 초기화 코드
}

// SQLite 데이터베이스 연결과 저장을 위한 함수
void save_score_to_db_snake(const char* player_name, int score) {
    sqlite3* db = nullptr;
    char* err_msg = nullptr;

    int rc = sqlite3_open("snake.db", &db);

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

void run_snake_game() {
    std::cout << "Running Snake Game...\n";
    bool quit = false;

    // 게임 루프 예시
    int score = 0;  // 먹은 횟수를 추적하는 변수

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    srand((unsigned int)time(NULL));

    Snake snake;
    Point food;
    init_snake(&snake);
    generate_food(&food, &snake);

    bool game_over = false;
    Uint32 last_move_time = SDL_GetTicks();

    while (!game_over) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game_over = true;
            }
            handle_input(&snake, &e);
        }

        if (SDL_GetTicks() - last_move_time > SNAKE_SPEED) {
            move_snake(&snake);
            int collision = check_collision(&snake, &food);
            if (collision == 1) {
                game_over = true;
            }
            else if (collision == 2) {
                generate_food(&food, &snake);
                score++;  // 음식을 먹을 때마다 카운트 증가
            }
            last_move_time = SDL_GetTicks();
        }

        render_game(renderer, &snake, &food);

        // 먹은 횟수 출력

        SDL_Delay(10);
    }
    SDL_Delay(2000);  // 2초간 점수 표시 후 게임 종료

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Food eaten: " << score << "\n";

    char player_name[50];
    printf("Enter your name: ");
    fgets(player_name, sizeof(player_name), stdin);

    // 줄바꿈 제거
    player_name[strcspn(player_name, "\n")] = 0;

    // 점수와 이름을 DB에 저장
    save_score_to_db_snake(player_name, score);
}
