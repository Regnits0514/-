#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "game.h"  // ��� ���� ����

// ���� �Լ�
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Game Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);  // ���⿡ ���� ��Ʈ ��θ� �����ϼ���.
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return -1;
    }

    // ���� �޴� ȭ�� ǥ��
    bool quit = false;
    bool in_game = false;  // ���� ���� ����
    bool game_over = false;  // ���� ���� ����

    SDL_Event e;
    int selected_option = 0;

    auto render_text = [](SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y) {
        SDL_Color color = { 255, 255, 255 }; // ���
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect destRect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        };

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (selected_option > 0) selected_option--;
                    break;
                case SDLK_DOWN:
                    if (selected_option < 2) selected_option++;
                    break;
                case SDLK_RETURN:
                    // Enter Ű�� ������ ���� ����
                    if (selected_option == 0) {
                        run_snake_game();  // Snake ���� ����
                    }
                    else if (selected_option == 1) {
                        run_breakout_game();  // Breakout ���� ����
                    }
                    else if (selected_option == 2) {
                        run_pong_game();  // Pong ���� ����
                    }
                    break;
                case SDLK_ESCAPE:
                    quit = true;  // ESC Ű�� ����
                    break;
                }
            }
        }

        // ���� ���� ���� ó��
        if (game_over) {
            render_text(renderer, font, "Game Over! Press Enter to return to the Main Menu.", 100, 350);
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);  // ��� ��� �� ���� �޴��� ���ư���
            game_over = false;  // ���� ���� �÷��� �ʱ�ȭ
            selected_option = 0; // ���� �޴� ù ��° �׸����� ���ư���
        }

        // ȭ�� �׸��� (���� �޴�)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ���� ����
        SDL_RenderClear(renderer);

        render_text(renderer, font, "Main Menu", 250, 50);  // ����
        render_text(renderer, font, "1. Snake Game", 250, 150);
        render_text(renderer, font, "2. Breakout Game", 250, 200);
        render_text(renderer, font, "3. Pong Game", 250, 250);

        // ���õ� �׸� ����
        SDL_Rect highlightRect = { 230, 140 + (selected_option * 50), 180, 40 }; // SDL_Rect�� ���� ����
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // ���������� ����
        SDL_RenderDrawRect(renderer, &highlightRect); // �ش� Rect�� �׸��ϴ�.

        SDL_RenderPresent(renderer);
        SDL_Delay(100);  // ȭ�� ���� �ӵ� ����
    }

    // SDL �ڿ� ����
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
