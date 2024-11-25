#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define CELL_SIZE 20
#define INITIAL_SNAKE_LENGTH 5
#define SNAKE_SPEED 25  // �и��� ������ �̵� �ֱ�

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[SCREEN_WIDTH * SCREEN_HEIGHT / (CELL_SIZE * CELL_SIZE)];
    int length;
    int dx, dy;  // �̵� ����
} Snake;

void init_snake(Snake* snake) {
    snake->length = INITIAL_SNAKE_LENGTH;
    snake->dx = CELL_SIZE;  // ���������� �̵� ����
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
    // �� �浹 �˻�
    if (snake->body[0].x < 0 || snake->body[0].x >= SCREEN_WIDTH || snake->body[0].y < 0 || snake->body[0].y >= SCREEN_HEIGHT)
        return 1;

    // �ڱ� ���� �浹 �˻�
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            return 1;
    }

    // ���� �Ա�
    if (snake->body[0].x == food->x && snake->body[0].y == food->y) {
        snake->length++;
        return 2;  // ���� ����
    }

    return 0;  // �浹 ����
}

void generate_food(Point* food, Snake* snake) {
    int valid = 0;
    while (!valid) {
        valid = 1;
        food->x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
        food->y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
        for (int i = 0; i < snake->length; i++) {
            if (food->x == snake->body[i].x && food->y == snake->body[i].y) {
                valid = 0;  // ������ ���� �� ���� ������ �ʵ���
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // ���� (����)
    SDL_RenderClear(renderer);

    // �� �׸���
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // �ʷϻ�
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = { snake->body[i].x, snake->body[i].y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    // ���� �׸���
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // ������
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