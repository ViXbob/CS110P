#ifndef __PICTOOL_H
#define __PICTOOL_H 

#include "lcd/lcd.h"

#define BOARD_HEIGHT 80
#define BOARD_WIDTH  160
#define RESERVED_BAND 19

static u8 whole_pic[BOARD_HEIGHT * BOARD_WIDTH];

u8 add_item(u8 *pic, u8 *item, u8 width, u8 height, int x, int y);

// Size: 160x80
// Top-left point is (0, 0), while bottom-right is (159, 79)
void show_pic(u8 *pic);

// add a rectangle
void add_rectangle(u8 *pic, u8 x, u8 y, u8 width, u8 height, u8 color);

// Add a char
void add_char(u8 *pic, u8 x, u8 y, u8 num, u8 color);

// Add a string
void add_string(u8 *pic, u8 x, u8 y, const u8 *p, u8 color, u8 space);

// set a pixel
void set_pixel(u8 *pic, u8 x, u8 y, u8 color);

// add random point with range
void add_random_point(u8 *pic, u8 x_l, u8 y_l, u8 x_r, u8 y_r, u8 num, u8 color);

// shift background
void shift_back_pic(u8 *pic, u8 shift);

// back pic
void back_up(u8 *pic, u8 *item, u8 width, u8 height, int x, int y);

#endif