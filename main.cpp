#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "game.h"  // 헤더 파일 포함

// 메인 함수
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

    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);  // 여기에 실제 폰트 경로를 설정하세요.
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return -1;
    }

    // 메인 메뉴 화면 표시
    bool quit = false;
    bool in_game = false;  // 게임 진행 여부
    bool game_over = false;  // 게임 오버 상태

    SDL_Event e;
    int selected_option = 0;

    auto render_text = [](SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y) {
        SDL_Color color = { 255, 255, 255 }; // 흰색
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
                    // Enter 키로 선택한 게임 실행
                    if (selected_option == 0) {
                        run_snake_game();  // Snake 게임 실행
                    }
                    else if (selected_option == 1) {
                        run_breakout_game();  // Breakout 게임 실행
                    }
                    else if (selected_option == 2) {
                        run_pong_game();  // Pong 게임 실행
                    }
                    break;
                case SDLK_ESCAPE:
                    quit = true;  // ESC 키로 종료
                    break;
                }
            }
        }

        // 게임 오버 상태 처리
        if (game_over) {
            render_text(renderer, font, "Game Over! Press Enter to return to the Main Menu.", 100, 350);
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);  // 잠시 대기 후 메인 메뉴로 돌아가기
            game_over = false;  // 게임 오버 플래그 초기화
            selected_option = 0; // 메인 메뉴 첫 번째 항목으로 돌아가기
        }

        // 화면 그리기 (메인 메뉴)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 배경색 검정
        SDL_RenderClear(renderer);

        render_text(renderer, font, "Main Menu", 250, 50);  // 제목
        render_text(renderer, font, "1. Snake Game", 250, 150);
        render_text(renderer, font, "2. Breakout Game", 250, 200);
        render_text(renderer, font, "3. Pong Game", 250, 250);

        // 선택된 항목 강조
        SDL_Rect highlightRect = { 230, 140 + (selected_option * 50), 180, 40 }; // SDL_Rect로 변수 선언
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 빨간색으로 강조
        SDL_RenderDrawRect(renderer, &highlightRect); // 해당 Rect를 그립니다.

        SDL_RenderPresent(renderer);
        SDL_Delay(100);  // 화면 갱신 속도 조절
    }

    // SDL 자원 해제
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
