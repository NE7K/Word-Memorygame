#include "game.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdio>

// 카드 뒤집기 (타이머 호출)
Uint32 flip_card(Uint32 interval, void* param) {
    static_cast<Card*>(param)->isRevealed = false;  // 카드 숨김
    return 0;
}

// 카드 숨기기 (힌트 버튼 클릭)
Uint32 hide_card(Uint32 interval, void* param) {
    static_cast<Card*>(param)->isRevealed = false;  // 카드 숨김
    return 0;
}

// 카드 생성자
Card::Card(SDL_Texture* tex, int cardId, int x, int y)
    : texture(tex), id(cardId), isRevealed(false), rect{ x, y, CARD_SIZE, CARD_SIZE } {}

// 게임 생성자
Game::Game(SDL_Renderer* renderer, const std::vector<std::string>& imagePaths, const std::string& hiddenImagePath)
    : currentCardIndex(0), mistakes(0), initialDisplay(true) {
    hiddenTexture = IMG_LoadTexture(renderer, hiddenImagePath.c_str());  // 뒷면 이미지
    for (int i = 0; i < CARD_COUNT; ++i) {
        SDL_Surface* surface = IMG_Load(imagePaths[i].c_str());  // 카드 이미지
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);  // 텍스처 생성
            SDL_FreeSurface(surface);  // 서피스 해제
            int x = (i % 4) * (CARD_SIZE + 10) + 50;  // X 좌표
            int y = (i / 4) * (CARD_SIZE + 10) + 50;  // Y 좌표
            cards.emplace_back(texture, i, x, y);  // 카드 추가
        }
    }
    shuffleCards();  // 카드 섞기
    startTime = SDL_GetTicks();  // 시작 시간
}

// 게임 소멸자
Game::~Game() {
    for (auto& card : cards) SDL_DestroyTexture(card.texture);  // 텍스처 해제
    SDL_DestroyTexture(hiddenTexture);  // 뒷면 텍스처 해제
}

// 카드 섞기
void Game::shuffleCards() {
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(static_cast<unsigned>(time(nullptr))));  // 카드 섞기
    int xOffset = 50, yOffset = 50, spacing = 10;
    for (size_t i = 0; i < cards.size(); ++i) {
        cards[i].rect.x = (i % 4) * (CARD_SIZE + spacing) + xOffset;  // X 좌표
        cards[i].rect.y = (i / 4) * (CARD_SIZE + spacing) + yOffset;  // Y 좌표
    }
}

// 카드 렌더링
void Game::render(SDL_Renderer* renderer) {
    for (const auto& card : cards) {
        SDL_RenderCopy(renderer, card.isRevealed || initialDisplay ? card.texture : hiddenTexture, nullptr, &card.rect);  // 카드 렌더링
    }
}

// 카드 클릭 처리
void Game::handleCardClick(int mouseX, int mouseY, SDL_Renderer* renderer) {
    SDL_Point mousePoint = { mouseX, mouseY };  // 클릭 좌표
    for (auto& card : cards) {
        if (SDL_PointInRect(&mousePoint, &card.rect)) {  // 클릭된 카드
            if (card.id == currentCardIndex) {  // 맞은 카드
                card.isRevealed = true;  // 카드 공개
                ++currentCardIndex;  // 다음 카드
                if (currentCardIndex == CARD_COUNT) {  // 게임 승리
                    printf("게임 승리!\n");
                    reset(renderer);  // 리셋
                }
            }
            else {  // 틀린 카드
                ++mistakes;  // 실수 증가
                if (mistakes >= MAX_MISTAKES) {  // 실수 초과
                    printf("실수 초과. 게임 리셋\n");
                    reset(renderer);  // 리셋
                }
            }
            break;
        }
    }
}

// 게임 리셋
void Game::reset(SDL_Renderer* renderer) {
    shuffleCards();  // 카드 섞기
    currentCardIndex = 0;  // 인덱스 초기화
    mistakes = 0;  // 실수 초기화
    initialDisplay = true;  // 초기 상태
    startTime = SDL_GetTicks();  // 시작 시간 갱신
    showAllCards(renderer, START_OPEN);  // 모든 카드 공개
}

// 카드 모두 공개
void Game::showAllCards(SDL_Renderer* renderer, Uint32 duration) {
    Uint32 start = SDL_GetTicks();  // 시작 시간
    while (SDL_GetTicks() - start < duration) {  // 일정 시간
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // 배경 색
        SDL_RenderClear(renderer);  // 화면 지우기
        for (const auto& card : cards) {
            SDL_RenderCopy(renderer, card.texture, nullptr, &card.rect);  // 카드 렌더링
        }
        SDL_RenderPresent(renderer);  // 화면 업데이트
        SDL_Delay(16);
    }
    for (auto& card : cards) {
        card.isRevealed = false;  // 카드 숨기기
    }
}
