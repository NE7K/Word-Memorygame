#include "ui.h"       
#include "constants.h"  


void UI::renderButton(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect rect, const std::string& text) {
    // 버튼 배경 색상
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect); // 버튼 영역 채우기

    // 버튼 테두리 색상 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect); // 버튼 테두리 그리기

    // 버튼 텍스트 
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor); 
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); 
    SDL_Rect textRect = {
        rect.x + (rect.w - textSurface->w) / 2, 
        rect.y + (rect.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// 점수 렌더링 함수
void UI::renderScore(SDL_Renderer* renderer, TTF_Font* font, const std::string& scoreText) {
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = { 10, SCREEN_HEIGHT - 50, scoreSurface->w, scoreSurface->h }; 

    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
}
