#ifndef GAME_H 
#define GAME_H

#include <SDL.h>       
#include <SDL_image.h>  
#include <vector>      
#include <string>
#include "constants.h"

// 카드 클래스
class Card {
public:
    SDL_Rect rect;         // 카드의 위치와 크기
    SDL_Texture* texture;  // 카드 이미지 
    int id;                // 카드 ID 
    bool isRevealed;       

    // 생성자
    Card(SDL_Texture* tex, int cardId, int x, int y);
};

// 게임 클래스
class Game {
private:
    std::vector<Card> cards;          // 카드 리스트
    SDL_Texture* hiddenTexture;       // 카드 뒷면 이미지
    int currentCardIndex;             // 맞춰야 할 카드 인덱스
    int mistakes;                     // 실수 횟수
    bool initialDisplay;              // 초기 카드 공개 여부
    Uint32 startTime;                 // 공개 시작 시간

public:
    Game(SDL_Renderer* renderer, const std::vector<std::string>& imagePaths, const std::string& hiddenImagePath);
    ~Game();

    void shuffleCards();                             // 카드 섞기
    void render(SDL_Renderer* renderer);             // 카드 렌더링
    void handleCardClick(int mouseX, int mouseY, SDL_Renderer* renderer);  // 카드 클릭 
    void reset(SDL_Renderer* renderer);              // 게임 리셋
    void showAllCards(SDL_Renderer* renderer, Uint32 duration); // 모든 카드 공개

    bool isInitialDisplay() const { return initialDisplay; } // 초기 공개 상태
    void setInitialDisplay(bool value) { initialDisplay = value; } // 초기 공개 상태
    Uint32 getStartTime() const { return startTime; } // 공개 시작 시간
    void setStartTime(Uint32 value) { startTime = value; } // 공개 시작 시간
    int getCurrentCardIndex() const { return currentCardIndex; } 
    void incrementCardIndex() { currentCardIndex++; }
    std::vector<Card>& getCards() { return cards; }   
};

#endif
