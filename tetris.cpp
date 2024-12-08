#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "game.h"
#include <SDL_ttf.h>
#include <string>
#include <sstream>

#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

// 화면 크기 및 블록 크기 정의
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 1000;
const int BLOCK_SIZE = 40;
const int ROWS = SCREEN_HEIGHT / BLOCK_SIZE;
const int COLS = SCREEN_WIDTH / BLOCK_SIZE;

int grid[ROWS][COLS] = { 0 }; // This is already declared globally.
int currentBlock[4][4]; // This is already declared globally.
int currentX = COLS / 2 - 2, currentY = 0; // Already declared globally.

int score2 = 0;                // 점수 변수

// 색상 정의
SDL_Color colors[] = {
    {0, 0, 0, 255},        // 빈 공간
    {255, 0, 0, 255},      // 빨강
    {0, 255, 0, 255},      // 초록
    {0, 0, 255, 255},      // 파랑
    {255, 255, 0, 255},    // 노랑
    {255, 0, 255, 255},    // 자홍
    {0, 255, 255, 255}     // 청록
};

// 테트로미노 정의
int tetrominoes[7][4][4] = {
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T-모양
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // I-모양
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // O-모양
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z-모양
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S-모양
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L-모양
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}  // J-모양
};

// 블록 회전 함수
void rotateBlock() {
    int temp[4][4] = { 0 };
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            temp[x][3 - y] = currentBlock[y][x];
        }
    }
    memcpy(currentBlock, temp, sizeof(temp));
}

// 충돌 감지
bool checkCollision(int dx, int dy) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x]) {
                int newX = currentX + x + dx;
                int newY = currentY + y + dy;
                if (newX < 0 || newX >= COLS || newY >= ROWS || grid[newY][newX]) {
                    return true;
                }
            }
        }
    }
    return false;
}

// 블록 고정
void placeBlock() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock[y][x]) {
                grid[currentY + y][currentX + x] = currentBlock[y][x];
            }
        }
    }
}

// 라인 제거
void clearLines() {
    for (int y = 0; y < ROWS; y++) {
        bool fullLine = true;
        for (int x = 0; x < COLS; x++) {
            if (!grid[y][x]) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            // 한 줄을 지울 때마다 점수 증가 (100점)
            score2 += 100;

            // 라인 제거 후 위로 밀기
            for (int row = y; row > 0; row--) {
                memcpy(grid[row], grid[row - 1], sizeof(grid[row]));
            }
            memset(grid[0], 0, sizeof(grid[0]));
        }
    }
}

// 랜덤 블록 생성
void spawnBlock() {
    int id = rand() % 7;
    memcpy(currentBlock, tetrominoes[id], sizeof(currentBlock));
    currentX = COLS / 2 - 2;
    currentY = 0;
}

// 게임 초기화
void initGame() {
    srand(time(0));
    spawnBlock();
}

void init_tetris_game() {
    std::cout << "Tetris Game Initialized.\n";
    // 초기화 코드
}

void displayScore(SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if (font == nullptr) {
        std::cout << "Failed to load font!" << std::endl;
        return;
    }

    SDL_Color textColor = { 255, 255, 255, 255 }; // white
    std::ostringstream oss;
    oss << score2;
    std::string score_str = oss.str();

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, ("Score: " + score_str).c_str(), textColor);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    int messageWidth = surfaceMessage->w;
    int messageHeight = surfaceMessage->h;
    SDL_Rect messageRect = { SCREEN_WIDTH / 2 - messageWidth / 2, SCREEN_HEIGHT / 2 - messageHeight / 2, messageWidth, messageHeight };

    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_RenderPresent(renderer);

    // Clean up resources
    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
    TTF_CloseFont(font);
}

// SQLite 데이터베이스 연결과 저장을 위한 함수
void save_score_to_db_tetris(const char* player_name, int score) {
    sqlite3* db = nullptr;
    char* err_msg = nullptr;

    int rc = sqlite3_open("tetris.db", &db);

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

void run_tetris_game() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();  // TTF 초기화

    SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks(), currentTime;

    initGame();

    while (running) {
        // 이벤트 처리
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    if (!checkCollision(-1, 0)) currentX--;
                    break;
                case SDLK_RIGHT:
                    if (!checkCollision(1, 0)) currentX++;
                    break;
                case SDLK_DOWN:
                    if (!checkCollision(0, 1)) currentY++;
                    break;
                case SDLK_UP:
                    rotateBlock();
                    if (checkCollision(0, 0)) rotateBlock(); // 회전 복구
                    break;
                }
            }
        }

        // 블록 떨어뜨리기
        currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 500) {
            if (!checkCollision(0, 1)) {
                currentY++;
            }
            else {
                placeBlock();
                clearLines();
                spawnBlock();
                if (checkCollision(0, 0)) {
                    running = false; // 게임 오버
                }
            }
            lastTime = currentTime;
        }

        // 화면 그리기
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 게임 보드 그리기
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (grid[y][x]) {
                    SDL_Rect rect = { x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                    SDL_SetRenderDrawColor(renderer, colors[grid[y][x]].r, colors[grid[y][x]].g, colors[grid[y][x]].b, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // 현재 블록 그리기
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (currentBlock[y][x]) {
                    SDL_Rect rect = { (currentX + x) * BLOCK_SIZE, (currentY + y) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                    SDL_SetRenderDrawColor(renderer, colors[currentBlock[y][x]].r, colors[currentBlock[y][x]].g, colors[currentBlock[y][x]].b, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // 게임 오버 시 점수 표시
        if (!running) {
            displayScore(renderer);

            SDL_Delay(2000);  // 2초간 점수 표시 후 게임 종료

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            TTF_Quit();

            // 이름 입력 받기
            char player_name[50];
            printf("Enter your name: ");
            fgets(player_name, sizeof(player_name), stdin);

            // 줄바꿈 제거
            player_name[strcspn(player_name, "\n")] = 0;

            // 점수와 이름을 DB에 저장

            save_score_to_db_tetris(player_name, score2);


        }

        SDL_RenderPresent(renderer);
    }
}
