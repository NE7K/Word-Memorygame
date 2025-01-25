#ifndef CONSTANTS_H
#define CONSTANTS_H

// 게임 화면 크기 설정
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// 게임 설정
const int CARD_COUNT = 8;       // 카드 개수
const int CARD_SIZE = 128;      // 카드 크기
const int START_OPEN = 4000;    // 게임 시작 시 카드 공개 시간
const int HINT_OPEN = 3000;     // 힌트 버튼 클릭 후 공개 시간
const int MAX_MISTAKES = 3;     // 허용되는 최대 실수 횟수
const int MAX_HINTS = 2;        // 힌트 버튼 최대 사용 횟수
const int MAX_NEXTS = 3;        // Next 버튼 최대 사용 횟수

#endif
