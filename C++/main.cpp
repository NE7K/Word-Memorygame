#include "main.h"
#include "ui.h"  
#include "game.h" 
#include <vector>
#include <string> 


int main(int argc, char* argv[]) {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        return 1;
    }

    // SDL 이미지 초기화
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("SDL_image 초기화 실패: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // SDL 폰트 초기화
    if (TTF_Init() == -1) {
        printf("SDL_ttf 초기화 실패: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // SDL 창 생성
    SDL_Window* window = SDL_CreateWindow(
        "Card Game 2020U2324",             
        SDL_WINDOWPOS_CENTERED,         
        SDL_WINDOWPOS_CENTERED,         
        SCREEN_WIDTH, SCREEN_HEIGHT,      
        SDL_WINDOW_SHOWN                 
    );


    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 20);

    // 카드 이미지 경로 설정
    std::vector<std::string> imagePaths = {
        "C:/Users/neigh/Desktop/test/test/img/word1.png",
        "C:/Users/neigh/Desktop/test/test/img/word2.png",
        "C:/Users/neigh/Desktop/test/test/img/word3.png",
        "C:/Users/neigh/Desktop/test/test/img/word4.png",
        "C:/Users/neigh/Desktop/test/test/img/word5.png",
        "C:/Users/neigh/Desktop/test/test/img/word6.png",
        "C:/Users/neigh/Desktop/test/test/img/word7.png",
        "C:/Users/neigh/Desktop/test/test/img/word8.png"
    };

    // 게임 객체
    Game game(renderer, imagePaths, "C:/Users/neigh/Desktop/test/test/img/word_behind.png");

    // 버튼 위치
    SDL_Rect exitButton = { SCREEN_WIDTH - 150, SCREEN_HEIGHT - 50, 100, 40 };    // 종료 버튼
    SDL_Rect restartButton = { SCREEN_WIDTH - 600, SCREEN_HEIGHT - 50, 100, 40 }; // 재시작 버튼
    SDL_Rect nextButton = { SCREEN_WIDTH - 450, SCREEN_HEIGHT - 50, 100, 40 };    // Next 버튼
    SDL_Rect hintButton = { SCREEN_WIDTH - 300, SCREEN_HEIGHT - 50, 100, 40 };    // 힌트 버튼


    bool quit = false;        // 게임 종료 
    SDL_Event event;
    float score = 0.0f;       // 점수
    int hintUsageCount = 0;   // 힌트 버튼 사용 횟수
    int nextUsageCount = 0;   // Next 버튼 사용 횟수
    bool isGameWon = false;   // 게임 승리 여부 체크
    char scoreText[50];       // 점수 텍스트 저장
    sprintf_s(scoreText, sizeof(scoreText), "Score: %.1f", score); // 점수 초기화

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks(); // 현재 시간 가져오기

        // 이벤트 처리 루프
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { // 종료 이벤트 처리
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) { // 마우스 클릭 처리
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                SDL_Point mousePoint = { mouseX, mouseY }; // 클릭 위치
                if (SDL_PointInRect(&mousePoint, &exitButton)) { // 종료 버튼
                    printf("게임 종료\n");
                    quit = true;
                }
                else if (SDL_PointInRect(&mousePoint, &restartButton)) { // 재시작 버튼
                    printf("게임 재시작\n");
                    game.reset(renderer); // 게임 초기화
                    hintUsageCount = 0;  // 힌트 초기화
                    nextUsageCount = 0;  // Next 초기화
                    score = 0.0f;        // 점수 초기화
                    isGameWon = false;   // 게임 승리 상태 초기화
                    sprintf_s(scoreText, sizeof(scoreText), "Score: %.1f", score); // 점수 텍스트 초기화
                }
                else if (SDL_PointInRect(&mousePoint, &nextButton)) { // Next 버튼
                    if (nextUsageCount < MAX_NEXTS) {
                        auto& cards = game.getCards();
                        if (game.getCurrentCardIndex() < CARD_COUNT) {
                            cards[game.getCurrentCardIndex()].isRevealed = true; // 카드 공개
                            SDL_AddTimer(2000, flip_card, &cards[game.getCurrentCardIndex()]); // 2초 뒤 카드 숨기기
                            game.incrementCardIndex(); // 다음 카드로 이동
                            nextUsageCount++;
                            score -= 1.0f; // 점수 차감
                            sprintf_s(scoreText, sizeof(scoreText), "Score: %.1f", score);
                        }
                    }
                }
                else if (SDL_PointInRect(&mousePoint, &hintButton)) { // 힌트 버튼
                    if (hintUsageCount < MAX_HINTS) {
                        auto& cards = game.getCards();
                        bool revealed = false;
                        while (!revealed) {
                            int randomIndex = rand() % CARD_COUNT; // 랜덤 카드 선택
                            if (!cards[randomIndex].isRevealed) {
                                cards[randomIndex].isRevealed = true; // 카드 공개
                                SDL_AddTimer(HINT_OPEN, hide_card, &cards[randomIndex]); // 카드 숨김 타이머
                                revealed = true;
                                hintUsageCount++;
                                score -= 1.0f; // 점수 차감
                                sprintf_s(scoreText, sizeof(scoreText), "Score: %.1f", score);
                            }
                        }
                    }
                }
                else {
                    game.handleCardClick(mouseX, mouseY, renderer); // 카드 클릭 처리
                }
            }
        }

        // 초기 공개 상태 종료
        if (game.isInitialDisplay() && currentTime - game.getStartTime() > START_OPEN) {
            game.setInitialDisplay(false);
            for (auto& card : game.getCards()) {
                card.isRevealed = false; // 모든 카드 숨기기
            }
        }

        // 화면 렌더링
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        game.render(renderer); // 게임 상태 렌더링
        UI::renderButton(renderer, font, exitButton, "Exit");        // 종료 버튼 렌더링
        UI::renderButton(renderer, font, restartButton, "Restart");  // 재시작 버튼 렌더링
        UI::renderButton(renderer, font, nextButton, "Next");        // Next 버튼 렌더링
        UI::renderButton(renderer, font, hintButton, "Random");      // 힌트 버튼 렌더링
        UI::renderScore(renderer, font, scoreText);                  // 점수 렌더링

        SDL_RenderPresent(renderer); // 렌더링 결과 출력
    }

    // 리소스 정리
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
