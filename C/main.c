#include <SDL.h> 
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <stdbool.h> 

#define card_count 8 // 카드 개수는 사전에 공지한 8개에 맞추기
#define width 800 
#define height 600 
#define card_size 128 // 카드의 크기 (scale 사용하지 않고 직접 제어)--> 오류 해결을 못하겠어서 교체
#define start_open 4000 // 게임 시작하면 카드 보여주는 시간
#define hint_open 3000 // 힌트 사용하면 3초
#define nonomistake 3 // 최대 실수
#define restart_open 4000 // 재시작 카드 보여주는 시간
#define how_next 3 // Next 맥스 사용 3
#define how_hint 2 // 힌트 맥스 사용 2


typedef struct {
    SDL_Rect rect; 
    SDL_Texture* texture;
    int id;
    bool is_revealed; 
} Card;

// 카드의 위치를 섞는 함수
void cardmix(Card* cards, int count) {
    for (int i = count - 1; i > 0; i--) { 
        int j = rand() % (i + 1);

        // 카드 위치 바꾸기
        SDL_Rect temp = cards[i].rect;
        cards[i].rect = cards[j].rect;
        cards[j].rect = temp; 
    }
}

// 게임을 재시작할 때 사용되는 함수
void reset_game(Card* cards, int* currentCardIndex, int* mistakes, SDL_Renderer* renderer) {
    *currentCardIndex = 0; 
    *mistakes = 0; 

    cardmix(cards, card_count); 

    Uint32 start = SDL_GetTicks(); 
    Uint32 current;
    while ((current = SDL_GetTicks()) - start < restart_open) { 
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
        SDL_RenderClear(renderer); 

        for (int i = 0; i < card_count; i++) { 
            SDL_RenderCopy(renderer, cards[i].texture, NULL, &cards[i].rect); 
        }

        SDL_RenderPresent(renderer); 
        SDL_Delay(16); 
    }

    for (int i = 0; i < card_count; i++) { 
        cards[i].is_revealed = false; 
    }

    printf("게임이 초기화되었습니다. 새로운 게임을 시작합니다.\n"); 
}

// 모든 카드를 일정 시간 동안 공개하는 함수
void open_card(Card* cards, Uint32 duration, SDL_Renderer* renderer) {
    Uint32 start = SDL_GetTicks(); 
    Uint32 current;

    while ((current = SDL_GetTicks()) - start < duration) { 
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < card_count; i++) {
            SDL_RenderCopy(renderer, cards[i].texture, NULL, &cards[i].rect); 
        }

        SDL_RenderPresent(renderer); 
        SDL_Delay(16);
    }
}

// 일정 시간 후에 카드를 숨기기
Uint32 hide_card(Uint32 interval, void* param) {
    Card* card = (Card*)param; 
    card->is_revealed = false; 
    return 0; 
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError()); 
        return 1;
    }

    if (!IMG_Init(IMG_INIT_PNG)) { 
        printf("SDL_Image 초기화 실패: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() == -1) { 
        printf("SDL_ttf 초기화 실패: %s\n", TTF_GetError()); 
        IMG_Quit(); 
        SDL_Quit(); 
        return 1;
    }

    // 윈도우 생성
    SDL_Window* window = SDL_CreateWindow(
        "2020U2324", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        SDL_WINDOW_SHOWN 
    );

    if (!window) { 
        printf("윈도우 생성 실패 : %s\n", SDL_GetError()); 
        TTF_Quit(); 
        IMG_Quit(); 
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("renderer 생성 실패 : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit(); 
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 20); 
    if (!font) { 
        printf("폰트 로드 실패 : %s\n", TTF_GetError()); 
        SDL_DestroyRenderer(renderer); 
        SDL_DestroyWindow(window); 
        TTF_Quit(); 
        IMG_Quit(); 
        SDL_Quit();
        return 1;
    }

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Rect exitButton = { width - 150, height - 50, 100, 40 }; 
    SDL_Rect hintButton = { width - 300, height - 50, 100, 40 }; 
    SDL_Rect nextButton = { width - 450, height - 50, 100, 40 }; 
    SDL_Rect restartButton = { width - 600, height - 50, 100, 40 };
    float score = 0.0f; // 초기 점수 설정

    // 점수 텍스트 설정
    char scoreText[50];
    sprintf_s(scoreText, sizeof(scoreText), "score : %.1f", score); 

    const char* imagePaths[card_count] = {
        "C:/Users/neigh/Desktop/test/test/img/word1.png", 
        "C:/Users/neigh/Desktop/test/test/img/word2.png", 
        "C:/Users/neigh/Desktop/test/test/img/word3.png", 
        "C:/Users/neigh/Desktop/test/test/img/word4.png", 
        "C:/Users/neigh/Desktop/test/test/img/word5.png", 
        "C:/Users/neigh/Desktop/test/test/img/word6.png",
        "C:/Users/neigh/Desktop/test/test/img/word7.png",
        "C:/Users/neigh/Desktop/test/test/img/word8.png"  
    };
    const char* hiddenImagePath = "C:/Users/neigh/Desktop/test/test/img/word_behind.png";

    // 카드 생성 및 초기화
    Card cards[card_count];
    SDL_Texture* hiddenTexture = NULL;

    for (int i = 0; i < card_count; i++) {
        SDL_Surface* surface = IMG_Load(imagePaths[i]); 
        if (!surface) { 
            printf("이미지 로드 실패 (%s) : %s\n", imagePaths[i], IMG_GetError()); // 디버깅용
            cards[i].texture = NULL; 
            continue; 
        }

        cards[i].texture = SDL_CreateTextureFromSurface(renderer, surface); // 카드 텍스처 생성
        SDL_FreeSurface(surface); 

        cards[i].id = i; // 카드 고유 번호 설정
        cards[i].is_revealed = false; 
        cards[i].rect.w = card_size; 
        cards[i].rect.h = card_size;
        cards[i].rect.x = (i % 4) * (card_size + 10) + 50; 
        cards[i].rect.y = (i / 4) * (card_size + 10) + 50;
    }

    // 숨겨진 카드 이미지 로드
    SDL_Surface* hiddenSurface = IMG_Load(hiddenImagePath);
    if (!hiddenSurface) { 
        printf("이미지 로드 실패 (%s) : %s\n", hiddenImagePath, IMG_GetError());
        hiddenTexture = NULL; 
    }
    else {
        hiddenTexture = SDL_CreateTextureFromSurface(renderer, hiddenSurface); 
        SDL_FreeSurface(hiddenSurface);
    }

    srand((unsigned int)time(NULL)); 

    cardmix(cards, card_count); // 게임 시작 시 카드 위치 섞기

    // 게임 상태
    int quit = 0; // 게임 종료
    int currentCardIndex = 0; // 맞춰야 할 카드
    int mistakes = 0; // 틀린 횟수
    bool initialDisplay = true; // 게임 시작 카드를 보여주는 상태
    bool gameOver = false; // 게임 오버인지 아닌지 체크
    Uint32 startTime = SDL_GetTicks();
    int nextUsageCount = 0; // Next 버튼 사용 횟수
    int hintUsageCount = 0; // 힌트 버튼 사용 횟수

    SDL_Event event;

    while (!quit) { 
        Uint32 currentTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_QUIT) { 
                quit = 1; 
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) { 
                int mouseX, mouseY; 
                SDL_GetMouseState(&mouseX, &mouseY); 

                SDL_Point mousePoint = { mouseX, mouseY }; 

                // 종료 버튼
                if (SDL_PointInRect(&mousePoint, &exitButton)) { 
                    printf("게임을 종료합니다.\n"); 
                    printf("총 점수는! %.1f점입니다.\n", score);
                    quit = 1; // 게임 종료 플래그 설정
                }

                // 힌트 버튼 
                if (!gameOver && hintUsageCount < how_hint && SDL_PointInRect(&mousePoint, &hintButton)) { 
                    bool revealed = false; 
                    while (!revealed) {
                        int randomIndex = rand() % card_count; // 랜덤하게 카드 선택
                        if (!cards[randomIndex].is_revealed) { 
                            cards[randomIndex].is_revealed = true; // 카드 공개
                            SDL_AddTimer(hint_open, hide_card, &cards[randomIndex]); // 3초 후 다시 숨기기
                            revealed = true; 
                            hintUsageCount++; // 힌트 사용 횟수 증가
                            score -= 1.0f; // 힌트 사용 시 -1점
                            sprintf_s(scoreText, sizeof(scoreText), "score : %.1f", score); // 점수 업데이트
                            printf("힌트 : 카드 %d이(가) 공개되었습니다. (%d/%d)\n", randomIndex, hintUsageCount, how_hint);
                        }
                    }
                }

                // Next 버튼 
                if (!gameOver && nextUsageCount < how_next && SDL_PointInRect(&mousePoint, &nextButton)) { 
                    if (currentCardIndex < card_count) { 
                        printf("Next 버튼 클릭됨. 다음 카드로 이동합니다. (%d/%d)\n", nextUsageCount + 1, how_next);
                        cards[currentCardIndex].is_revealed = true;
                        currentCardIndex++;
                        nextUsageCount++;
                        score -= 1.0f; 
                        sprintf_s(scoreText, sizeof(scoreText), "score : %.1f", score);
                    }
                }

                // 재시작 버튼 
                if (SDL_PointInRect(&mousePoint, &restartButton)) { 
                    printf("게임을 재시작합니다.\n"); 
                    reset_game(cards, &currentCardIndex, &mistakes, renderer); // 게임 상태 초기화
                    initialDisplay = true; 
                    startTime = SDL_GetTicks(); 
                    nextUsageCount = 0; // Next 버튼 사용 횟수
                    hintUsageCount = 0; // 힌트 버튼 사용 횟수
                }

                // 카드 클릭 처리
                if (!initialDisplay && !gameOver) { 
                    for (int i = 0; i < card_count; i++) { 
                        if (SDL_PointInRect(&mousePoint, &cards[i].rect)) { 
                            if (cards[i].id == currentCardIndex) { // 정답 카드 클릭 시
                                cards[i].is_revealed = true; // 카드 공개
                                currentCardIndex++; // 다음 카드로 이동

                                if (currentCardIndex == card_count) { // 모든 카드를 맞춘 경우
                                    score += 10.0f; // 클리어 시 10점 추가
                                    sprintf_s(scoreText, sizeof(scoreText), "score : %.1f", score);
                                    printf("승리했습니다! 점수 : %.1f. 게임을 재시작합니다.\n", score);
                                    reset_game(cards, &currentCardIndex, &mistakes, renderer); // 게임 초기화
                                    initialDisplay = true; // 초기 카드 공개 상태로 설정
                                    startTime = SDL_GetTicks(); 
                                    nextUsageCount = 0; // Next 횟수 초기화
                                    hintUsageCount = 0; // 힌트 횟수 초기화
                                }
                            }
                            else { // 잘못된 카드 클릭 시
                                score -= 0.5f; 
                                mistakes++; 
                                sprintf_s(scoreText, sizeof(scoreText), "score : %.1f", score); 
                                printf("잘못된 카드를 클릭했습니다! 점수 : %.1f. 틀림 횟수 : %d/%d\n", score, mistakes, nonomistake); 

                                if (mistakes >= nonomistake) { // 틀린 횟수가 최대치를 초과한 경우
                                    printf("실수가 너무 많아요..! 모든 카드를 공개합니다.\n"); 
                                    open_card(cards, hint_open, renderer); 
                                    reset_game(cards, &currentCardIndex, &mistakes, renderer); 
                                    initialDisplay = true; 
                                    startTime = SDL_GetTicks(); // 시작 시간 초기화
                                    nextUsageCount = 0; // Next 버튼 사용 횟수 초기화
                                    hintUsageCount = 0; // 힌트 버튼 사용 횟수 초기화
                                }
                            }
                            break; 
                        }
                    }
                }
            }
        }

        // 초기 카드 공개 시간이 지나면 카드를 숨김
        if (initialDisplay && currentTime - startTime > start_open) {
            initialDisplay = false; 
            for (int i = 0; i < card_count; i++) {
                cards[i].is_revealed = false;
            }
        }

        // 화면
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer); 

        // 카드 그리기
        for (int i = 0; i < card_count; i++) {
            if (cards[i].is_revealed || initialDisplay) {
                SDL_RenderCopy(renderer, cards[i].texture, NULL, &cards[i].rect); 
            }
            else if (hiddenTexture) { 
                SDL_RenderCopy(renderer, hiddenTexture, NULL, &cards[i].rect); 
            }
        } 


        // 여기서 부터는 ui 부분



        // 종료 버튼
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 버튼 배경색
        SDL_RenderFillRect(renderer, &exitButton); // 종료 버튼
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 테두리 색상
        SDL_RenderDrawRect(renderer, &exitButton); // 테두리

        // Exit 텍스트
        SDL_Surface* exitTextSurface = TTF_RenderText_Solid(font, "Exit", textColor); 
        SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
        SDL_Rect exitTextRect = { exitButton.x + 25, exitButton.y + 10, exitTextSurface->w, exitTextSurface->h }; 
        SDL_RenderCopy(renderer, exitTextTexture, NULL, &exitTextRect);
        SDL_FreeSurface(exitTextSurface); 
        SDL_DestroyTexture(exitTextTexture);

        // 힌트 버튼
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
        SDL_RenderFillRect(renderer, &hintButton); 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderDrawRect(renderer, &hintButton); 

        // Random 텍스트
        SDL_Surface* hintTextSurface = TTF_RenderText_Solid(font, "Random", textColor); 
        SDL_Texture* hintTextTexture = SDL_CreateTextureFromSurface(renderer, hintTextSurface); 
        SDL_Rect hintTextRect = { hintButton.x + 20, hintButton.y + 10, hintTextSurface->w, hintTextSurface->h }; 
        SDL_RenderCopy(renderer, hintTextTexture, NULL, &hintTextRect); 
        SDL_FreeSurface(hintTextSurface); 
        SDL_DestroyTexture(hintTextTexture); 

        // Next 버튼
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &nextButton);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderDrawRect(renderer, &nextButton); 

        // Next 텍스트
        SDL_Surface* nextTextSurface = TTF_RenderText_Solid(font, "Next", textColor);
        SDL_Texture* nextTextTexture = SDL_CreateTextureFromSurface(renderer, nextTextSurface); 
        SDL_Rect nextTextRect = { nextButton.x + 25, nextButton.y + 10, nextTextSurface->w, nextTextSurface->h }; 
        SDL_RenderCopy(renderer, nextTextTexture, NULL, &nextTextRect);
        SDL_FreeSurface(nextTextSurface); 
        SDL_DestroyTexture(nextTextTexture); 

        // 재시작 버튼
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
        SDL_RenderFillRect(renderer, &restartButton); 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderDrawRect(renderer, &restartButton);

        // Restart 텍스트
        SDL_Surface* restartTextSurface = TTF_RenderText_Solid(font, "Restart", textColor);
        SDL_Texture* restartTextTexture = SDL_CreateTextureFromSurface(renderer, restartTextSurface);
        SDL_Rect restartTextRect = { restartButton.x + 10, restartButton.y + 10, restartTextSurface->w, restartTextSurface->h };
        SDL_RenderCopy(renderer, restartTextTexture, NULL, &restartTextRect);
        SDL_FreeSurface(restartTextSurface);
        SDL_DestroyTexture(restartTextTexture);

        // 점수 표시
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = { 10, height - 50, scoreSurface->w, scoreSurface->h };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurface); 
        SDL_DestroyTexture(scoreTexture);

        SDL_RenderPresent(renderer);
    }

    // 종료해야 하는 것들
    TTF_CloseFont(font);
    for (int i = 0; i < card_count; i++) {
        if (cards[i].texture) {
            SDL_DestroyTexture(cards[i].texture);
        }
    }

    if (hiddenTexture) {
        SDL_DestroyTexture(hiddenTexture);
    }
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    TTF_Quit();
    IMG_Quit(); 
    SDL_Quit();

    return 0; 
}
