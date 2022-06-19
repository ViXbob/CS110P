#include "pictool.h"
#include "color.h"
#include "font.h"
#include <time.h>
#include <stdlib.h>

// u32 whole_pic[BOARD_HEIGHT * BOARD_WIDTH] = {0};

// add a item
// return value 0: no covery
// return value 1: covery
u8 add_item(u8 *pic, u8 *item, u8 width, u8 height, int x, int y) {
    u8 return_flag = 0;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            // pic[(i + loc_i) * BOARD_WIDTH + j + loc_j] |= (item[i * width + j] << shift);
            if(i + y >= 0 && i + y < BOARD_HEIGHT && j + x >= 0 && j + x < BOARD_WIDTH) {
                if(pic[(i + y) * BOARD_WIDTH + j + x] && item[i * width + j]) return_flag = 1;
                set_pixel(pic, j + x, i + y, item[i * width + j]);
            }
        }
    }
    return return_flag;
}

void back_up(u8 *pic, u8 *item, u8 width, u8 height, int x, int y) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(i + y >= 0 && i + y < BOARD_HEIGHT && j + x >= 0 && j + x < BOARD_WIDTH) {
                item[i * width + j] = pic[(i + y) * BOARD_WIDTH + j + x];
            }
        }
    }
}

// Size: 160x80
// Top-left point is (0, 0), while bottom-right is (159, 79)
void show_pic(u8 *pic) {
    LCD_Address_Set(0, 0, BOARD_WIDTH - 1, BOARD_HEIGHT - 1);
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            LCD_WR_DATA(color_map[pic[i * BOARD_WIDTH + j]]);
        }
    }
}

// add a rectangle
void add_rectangle(u8 *pic, u8 x, u8 y, u8 width, u8 height, u8 color) {
    for(int i = y; i < y + height; i++) {
        for(int j = x; j < x + width; j++) {
            set_pixel(pic, j, i, color);
        }
    }
}

// add a char
void add_char(u8 *pic, u8 x, u8 y, u8 num, u8 color) {
    u8 temp;
    u8 pos, t;
    u8 x0 = x;   
    if(x > BOARD_WIDTH - 16 || y > BOARD_HEIGHT - 8) return;	    //Settings window		
    my_asc2_1608 = font();   
	num = num - ' '; //Get offset value
    for(pos = 0; pos < 16; pos++) { 
        temp = my_asc2_1608[(u16)num * 16 + pos];		 // Call 1608 font
        for(t = 0; t < 8; t++) {                 
            if(temp & 0x01) set_pixel(pic, x, y, color); // LCD_WR_DATA(color);
            else set_pixel(pic, x, y, MY_BACK_COLOR); // LCD_WR_DATA(BACK_COLOR);
            temp >>= 1;
            x++;
        }
        x = x0;
        y++;
    }		   	 	  
}

// add a string
// 0 <= x < 160
// 0 <= y < 80
void add_string(u8 *pic, u8 x, u8 y, const u8 *p, u8 color, u8 space) {
    while(*p != '\0') {       
        add_char(pic, x, y, *p, color);
        x += 8 + space;
        p++;
    }  
}

// add random point to background
void add_random_point(u8 *pic, u8 x_l, u8 y_l, u8 x_r, u8 y_r, u8 num, u8 color) {
    u8 x, y;
    while(num--) {
        x = rand() % (x_r - x_l) + x_l;
        y = rand() % (y_r - y_l) + y_l;
        set_pixel(pic, x, y, color);
    }
}

// shift background
void shift_back_pic(u8 *pic, u8 shift) {
    for(int i = RESERVED_BAND; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH - shift; j++) {
            pic[i * BOARD_WIDTH + j] = pic[i * BOARD_WIDTH + j + shift];
        }
    }

}

// set a pixel
void set_pixel(u8 *pic, u8 x, u8 y, u8 color) {
    pic[y * BOARD_WIDTH + x] = color;
}