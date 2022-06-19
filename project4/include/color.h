#ifndef __COLOR_H
#define __COLOR_H 
#include "lcd/lcd.h"

#define MY_BLACK         0
#define MY_WHITE         1
#define MY_GBLUE         2
#define WORD_SHIFT       0
#define DINOSAUR_SHIFT   4
#define OBSTACLE_SHIFT   8
#define GROUND_SHIFT     12

static const u16 color_map[16] = {BLACK, WHITE, GBLUE};

static u8 MY_BACK_COLOR = 0;

#endif
