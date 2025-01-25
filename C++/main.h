#ifndef MAIN_H
#define MAIN_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "constants.h"  // 상수 정의 포함

// 카드 뒤집기 및 숨김 처리 함수 선언
Uint32 flip_card(Uint32 interval, void* param);  // 카드 뒤집기
Uint32 hide_card(Uint32 interval, void* param);  // 카드 숨기기

#endif
