#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <SDL_ttf.h>

// render_text �Լ� ����
void render_text(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const char* text);

#endif // COMMON_H
