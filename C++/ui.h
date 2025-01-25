#ifndef UI_H 
#define UI_H 

#include <SDL.h>       
#include <SDL_ttf.h>   
#include <string>     

namespace UI {
    // 버튼을 화면에 렌더링하는 함수
    void renderButton(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect rect, const std::string& text);

    // 점수를 화면에 렌더링하는 함수
    void renderScore(SDL_Renderer* renderer, TTF_Font* font, const std::string& scoreText);
}

#endif
