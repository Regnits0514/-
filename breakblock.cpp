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

// ���� ����
int ballVelX = BALL_SPEED, ballVelY = -BALL_SPEED;
int score = 0;
int lives = 3;
SDL_Rect paddle;
SDL_Rect ball;
SDL_Rect blocks[BLOCK_ROWS][BLOCK_COLS];

// ���� �ʱ�ȭ
void init_game() {
    paddle = { SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIGHT - 20, PADDLE_WIDTH, PADDLE_HEIGHT };
    ball = { SCREEN_WIDTH / 2 - BALL_RADIUS, SCREEN_HEIGHT - 100 - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2 };


    // ��� �迭 �ʱ�ȭ
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            blocks[i][j] = { j * (BLOCK_WIDTH + 5) + 30, i * (BLOCK_HEIGHT + 5) + 30, BLOCK_WIDTH, BLOCK_HEIGHT };
        }
    }
}

// �ؽ�Ʈ ������ �Լ�
void render_text(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const char* text) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, { 255, 255, 255, 255 });
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// ��, �е�, ��� �׸���
void draw_game(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // �е�
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &paddle);

    // ��
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball);

    // ���
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].w > 0) {
                SDL_RenderFillRect(renderer, &blocks[i][j]);
            }
        }
    }

    // ���� �� ���� ǥ��
    char scoreText[50];
    sprintf_s(scoreText, sizeof(scoreText), "Score: %d Lives: %d", score, lives);


    render_text(renderer, font, 10, 10, scoreText);

    SDL_RenderPresent(renderer);
}

// ���� ��� �浹 üũ
void check_collisions() {
    // ��ϰ��� �浹
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].w > 0) { // ����� Ȱ��ȭ�� ������ ����
                if (SDL_HasIntersection(&ball, &blocks[i][j])) {
                    blocks[i][j].w = 0; // ��� ����
                    blocks[i][j].h = 0;
                    score += 10; // ���� ����
                    ballVelY = -ballVelY; // �� ���� ����
                }
            }
        }
    }

    // �� �浹
    if (ball.x <= 0 || ball.x + ball.w >= SCREEN_WIDTH) {
        ballVelX = -ballVelX;
    }
    if (ball.y <= 0) {
        ballVelY = -ballVelY;
    }

    // �е�� �浹
    if (SDL_HasIntersection(&ball, &paddle)) {
        ballVelY = -ballVelY;
    }

    // �ٴڿ� ����� ��
    if (ball.y + ball.h >= SCREEN_HEIGHT) {
        lives--;
        if (lives > 0) {
            ball.x = SCREEN_WIDTH / 2 - BALL_RADIUS;
            paddle.x = SCREEN_WIDTH / 2 - BALL_RADIUS;
            ball.y = SCREEN_HEIGHT - 100 - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2;
        }
        else {
            // ���� ����
            printf("Game Over! Final Score: %d\n", score);
        }
    }
}

// ���� �Լ�
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

        // Ű �Է� ó�� (�¿�� �е� �̵�)
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

        // �� �̵�
        ball.x += ballVelX;
        ball.y += ballVelY;

        // �浹 ó��
        check_collisions();

        // ���� �׸���
        draw_game(renderer, font);

        SDL_Delay(16); // �ణ�� �����̸� �ξ� FPS�� 60���� ����
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
