#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    srand((unsigned int)time(NULL));

    Snake snake;
    Point food;
    init_snake(&snake);
    generate_food(&food, &snake);

    int game_over = 0;
    Uint32 last_move_time = SDL_GetTicks();

    while (!game_over) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game_over = 1;
            }
            handle_input(&snake, &e);
        }

        if (SDL_GetTicks() - last_move_time > SNAKE_SPEED) {
            move_snake(&snake);
            int collision = check_collision(&snake, &food);
            if (collision == 1) {
                game_over = 1;
            }
            else if (collision == 2) {
                generate_food(&food, &snake);
            }
            last_move_time = SDL_GetTicks();
        }

        render_game(renderer, &snake, &food);
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}