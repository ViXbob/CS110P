#include "lcd/lcd.h"
#include "utils.h"
#include "color.h"
#include "enumeration.h"
#include "pictool.h"
#include "font.h"
#include "item.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void Inp_init(void); 
// {
//     gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
// }

void Adc_init(void)  {
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
    RCU_CFG0|=(0b10<<14)|(1<<28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void);
// {
//     Inp_init(); // inport init
//     Adc_init(); // A/D init
//     Lcd_Init(); // LCD init
// }

// check whether game is over
// return value 0: dino didn't die
// return value 1: dino died
u8 check_die(u8 *pic) {
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            if((pic[i * BOARD_WIDTH + j] & 3) == 3) return 1;
        }
    }
    return 0;
}

#define BUTTON_DELAY                150
#define HEIGHT_PER_FRAME              7       // 32 / (40 / 5) * 2
#define JUMP_FRAME                   17
#define GROUND_TOP                   65
#define INITIAL_STONE                15
#define AVAILABLE_BIRD_Y_LIMIT        3
#define EASY_BIRD_GENERATED_INTERVAL 15
#define HARD_BIRD_GENERATED_INTERVAL 20
#define EASY_FRAME_SPEED              7
#define HARD_FRAME_SPEED              3
#define EASY_SCORE_PER_FRAME          3
#define HARD_SCORE_PER_FRAME          7
#define SINGLE_OPERATION_FRAME        10

static int BIRD_GENERATED_INTERVAL = 20;
static int CACTUS_GENERATED_INTERVAL = 25;
static int FRAME_SPEED = 5;
static int SCORE_PER_FRAME = 3;

static u8 AVAILABLE_BIRD_Y[AVAILABLE_BIRD_Y_LIMIT] = {GROUND_TOP - BIRD_HEIGHT - 3, GROUND_TOP - DINO_HEIGHT - BIRD_HEIGHT - 3, GROUND_TOP - PRONE_DINO_HEIGHT - BIRD_HEIGHT - 3};

u32 frame = 0;
u32 last_obstacle_frame = 0, last_updated_seed = 0;
u8 GAME_OVER = 0, Jump = 0;
u8 Button0 = 0, Button1 = 0, Boot = 0;
u8 score_string[10] = {0};
int BirdX[3] = {0}, BirdY[3] = {0}, BirdTime[3] = {0}, bird_index = 0, bird_num = 0, score = 0;
unsigned int input = 19260817;
GameState CurState = StartState;
GameDifficulty Difficulty = Easy;

void add_bird(u8 *tmp_pic, u8 width, u8 height) {
    for(int i = 0; i < bird_num; i++) {
        int index = (i + bird_index) % BIRD_LIMIT;
        u8 *ptr = tmp_pic + index * BIRD_WIDTH * BIRD_HEIGHT;
        back_up(whole_pic, ptr, BIRD_WIDTH, BIRD_HEIGHT, BirdX[index], BirdY[index]);
        add_item(whole_pic, bird[BirdTime[index] & 1], BIRD_WIDTH, BIRD_HEIGHT, BirdX[index], BirdY[index]);
    }
}

void add_bird_auto(u8 *tmp_pic, u8 width, u8 height) {
    for(int i = 0; i < bird_num; i++) {
        int index = (i + bird_index) % BIRD_LIMIT;
        u8 *ptr = tmp_pic + index * BIRD_WIDTH * BIRD_HEIGHT;
        back_up(whole_pic, ptr, BIRD_WIDTH, BIRD_HEIGHT, BirdX[index], BirdY[index]);
        add_item(whole_pic, bird[0], BIRD_WIDTH, BIRD_HEIGHT, BirdX[index], BirdY[index]);
    }
}

u8 add_dinosaur(u8 *tmp_pic, u8 width, u8 height) {
    if(Jump) {
        u8 tmp = (Jump > JUMP_FRAME / 2) ? (JUMP_FRAME - Jump) : Jump;
        back_up(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT - tmp * HEIGHT_PER_FRAME);
        return add_item(whole_pic, naive_white_dinosaur, DINO_WIDTH, DINO_HEIGHT, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT - tmp * HEIGHT_PER_FRAME);
    } else {
        if(Button1) {
            back_up(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - PRONE_DINO_HEIGHT);
            return add_item(whole_pic, prone_dinosaur[frame & 1], PRONE_DINO_WIDTH, PRONE_DINO_HEIGHT, DINO_BEGIN_X, GROUND_TOP - PRONE_DINO_HEIGHT);
        } else {
            back_up(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT);
            return add_item(whole_pic, stand_dinosaur[frame & 1], DINO_WIDTH, DINO_HEIGHT, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT);
        }
    }
    return 0;
}

void recovery_bird(u8 *tmp_pic, u8 width, u8 height) {
    for(int i = 0; i < bird_num; i++) {
        int index = (i + bird_index) % BIRD_LIMIT;
        u8 *ptr = tmp_pic + index * BIRD_WIDTH * BIRD_HEIGHT;
        add_item(whole_pic, ptr, BIRD_WIDTH, BIRD_HEIGHT, BirdX[index], BirdY[index]);
    }
}

void recovery_dinosaur(u8 *tmp_pic, u8 width, u8 height) {
    if(Jump) {
        u8 tmp = (Jump > JUMP_FRAME / 2) ? (JUMP_FRAME - Jump) : Jump;
        add_item(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT - tmp * HEIGHT_PER_FRAME);
    } else {
        if(Button1) {
            add_item(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - PRONE_DINO_HEIGHT);
        } else {
            add_item(whole_pic, tmp_pic, width, height, DINO_BEGIN_X, GROUND_TOP - DINO_HEIGHT);
        }
    }
}

void update_input_seed() {
    input <<= 1;
    input |= Button0;
    input <<= 1;
    input |= Button1;
    input ^= rand();
    if(frame - last_updated_seed > 200) {
        srand(input);
        last_updated_seed = frame;
    }
}

void update_bird() {
    u8 new_bird_index = bird_index, new_bird_num = bird_num;
    for(int i = 0; i < bird_num; i++) {
        int index = (i + bird_index) % BIRD_LIMIT;
        if(BirdX[index] + BIRD_WIDTH < FRAME_SPEED) {
            new_bird_index++;
            new_bird_num--;
        } else {
            BirdX[index] -= FRAME_SPEED;
            BirdTime[index]++;
        }
    }
    bird_index = new_bird_index;
    bird_num = new_bird_num;
}

void generate_obstacle() {
    if(!(rand() % 3)) add_random_point(whole_pic, BOARD_WIDTH - FRAME_SPEED, GROUND_TOP + 1, BOARD_WIDTH, BOARD_HEIGHT, 1, MY_WHITE);
    
    if(frame - last_obstacle_frame >= BIRD_GENERATED_INTERVAL && (rand() % 9) == 2 && bird_num < BIRD_LIMIT) {
        last_obstacle_frame = frame;
        int index = (bird_index + bird_num) % BIRD_LIMIT;
        bird_num++;
        BirdX[index] = BOARD_WIDTH;
        BirdY[index] = AVAILABLE_BIRD_Y[rand() % AVAILABLE_BIRD_Y_LIMIT];
        BirdTime[index] = 0;
    } else if(frame - last_obstacle_frame >= CACTUS_GENERATED_INTERVAL && (rand() % 9) == 1) {
        last_obstacle_frame = frame;
        add_item(whole_pic, cactus[rand() % 8], CACTUS_WIDTH, CACTUS_HEIGHT, CACTUS_BEGIN_X, GROUND_TOP - CACTUS_HEIGHT);
    }
}

void get_button_input() {
    Button0 = Get_Button(0);
    Button1 = Get_Button(1);
    if(Jump == 0) 
        Jump = Button0;
    else {
        Jump = (Jump + 1) % JUMP_FRAME;
    }
}

void InitGame() {
    frame = 0;
    last_obstacle_frame = 0; last_updated_seed = 0;
    GAME_OVER = 0; Jump = 0;
    Button0 = 0; Button1 = 0; Boot = 0;
    input = 19260817;
    bird_index = 0;
    bird_num = 0;
    score = 0;
    memset(BirdX, 0, sizeof(BirdX));
    memset(BirdY, 0, sizeof(BirdY));
    memset(BirdTime, 0, sizeof(BirdTime));
    memset(whole_pic, 0, sizeof(whole_pic));
    add_rectangle(whole_pic, 0, GROUND_TOP, BOARD_WIDTH, 1, MY_WHITE);
    add_random_point(whole_pic, 0, GROUND_TOP + 1, BOARD_WIDTH, BOARD_HEIGHT, INITIAL_STONE, MY_WHITE);
}

void show_score_and_mode() {
    // if(Difficulty == Easy) add_string(whole_pic, 10, 3, (u8*)("Easy"), MY_GBLUE, 0);
    // else add_string(whole_pic, 10, RESERVED_BAND - 16, (u8*)("Hard"), MY_GBLUE, 0);
    int len = 0;
    sprintf((char*)score_string, "%d", score);
    add_rectangle(whole_pic, BOARD_WIDTH - 50, RESERVED_BAND - 16, 50, 16, MY_BLACK);
    add_string(whole_pic, BOARD_WIDTH - 50, RESERVED_BAND - 16, score_string, MY_GBLUE, 1);
}

void set_difficulty(GameDifficulty Difficulty) {
    if(Difficulty == Hard) {
        BIRD_GENERATED_INTERVAL = HARD_BIRD_GENERATED_INTERVAL;
        FRAME_SPEED = HARD_FRAME_SPEED;
        SCORE_PER_FRAME = HARD_SCORE_PER_FRAME;
    } else {
        BIRD_GENERATED_INTERVAL = EASY_BIRD_GENERATED_INTERVAL;
        FRAME_SPEED = EASY_FRAME_SPEED;
        SCORE_PER_FRAME = EASY_SCORE_PER_FRAME;
    }
}

u8 frozen_frame = 0, cur_dinosaur_state = 0;
static u8 FROZEN_FRAME[3] = {2, 6, JUMP_FRAME - 1};
static u8 auto_tmp_pic[38 * 16];

void add_title() {
    back_up(whole_pic, auto_tmp_pic, 38, 16, 61, 25);
    if(Difficulty == Hard) {
        add_string(whole_pic, 61, 25, (u8*)("HARD"), MY_GBLUE, 2);
    } else {
        add_string(whole_pic, 61, 25, (u8*)("EASY"), MY_GBLUE, 2);
    }
}

void recovery_title() {
    add_item(whole_pic, auto_tmp_pic, 38, 16, 61, 25);
}

u8 check(u8 *pic, u8 *item, u8 width, u8 height, int x, int y) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(i + y >= 0 && i + y < BOARD_HEIGHT && j + x >= 0 && j + x < BOARD_WIDTH) {
                if(pic[(i + y) * BOARD_WIDTH + j + x] && item[i * width + j]) return 1;
            }
        }
    }
    return 0;
}

u8 detect(u8 state) {
    if(state == 0) {
        for(int i = 0; i < FROZEN_FRAME[state] * 4; i++) {
            if(check(whole_pic, stand_dinosaur[0], DINO_WIDTH, DINO_HEIGHT, DINO_BEGIN_X + FRAME_SPEED * i, GROUND_TOP - DINO_HEIGHT)) return 0;
        }
    } else if(state == 1) {
        for(int i = 0; i < FROZEN_FRAME[state]; i++) {
            if(check(whole_pic, prone_dinosaur[0], PRONE_DINO_WIDTH, PRONE_DINO_HEIGHT, DINO_BEGIN_X + FRAME_SPEED * i, GROUND_TOP - PRONE_DINO_HEIGHT)) return 0;
        }
    } else if(state == 2) {
        for(int i = 1; i <= FROZEN_FRAME[state]; i++) {
            int tmp = (i > JUMP_FRAME / 2) ? (JUMP_FRAME - i) : i;
            if(check(whole_pic, naive_white_dinosaur, DINO_WIDTH, DINO_HEIGHT, DINO_BEGIN_X + FRAME_SPEED * (i - 1), GROUND_TOP - DINO_HEIGHT - tmp * HEIGHT_PER_FRAME)) return 0;
        }
    }
    return 1;
}

void AutoPlay() {
    while(1) {
        add_bird_auto(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
        if(frozen_frame == 0) {
            cur_dinosaur_state = 4;
            if(detect(0)) {
                cur_dinosaur_state = 0;
                frozen_frame = FROZEN_FRAME[0];
            } else if(detect(1)) {
                cur_dinosaur_state = 1;
                frozen_frame = FROZEN_FRAME[1];
            } else if(detect(2)) {
                cur_dinosaur_state = 2;
                frozen_frame = FROZEN_FRAME[2];
            }
            if(cur_dinosaur_state == 0) {
                Button0 = 0; Button1 = 0; Jump = 0;
                frozen_frame = FROZEN_FRAME[0];
            } else if(cur_dinosaur_state == 1) {
                Button0 = 0; Button1 = 1; Jump = 0;
                frozen_frame = FROZEN_FRAME[1];
            } else if(cur_dinosaur_state == 2) {
                Button0 = 1; Button1 = 0; Jump = 1;
                frozen_frame = FROZEN_FRAME[2];
            } else {
                // delay_1ms(3000);
            }
        }
        recovery_bird(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
        add_bird(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
        GAME_OVER = add_dinosaur(dinosaur_tmp_pic, PRONE_DINO_WIDTH, DINO_HEIGHT);
        if(GAME_OVER) {
            InitGame();
            frozen_frame = 0;
            continue;
        }
        add_title();
        show_pic(whole_pic);
        recovery_bird(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
        recovery_dinosaur(dinosaur_tmp_pic, PRONE_DINO_WIDTH, DINO_HEIGHT);
        recovery_title();
        shift_back_pic(whole_pic, FRAME_SPEED);
        add_rectangle(whole_pic, BOARD_WIDTH - FRAME_SPEED, 0, FRAME_SPEED, BOARD_HEIGHT, MY_BLACK);
        add_rectangle(whole_pic, BOARD_WIDTH - FRAME_SPEED, GROUND_TOP, FRAME_SPEED, 1, MY_WHITE);
        frame++;
        frozen_frame--;
        if(Jump) {
            Jump = (Jump + 1) % JUMP_FRAME;
        }
        update_bird();
        generate_obstacle();
        update_input_seed();
        delay_1ms(30);
        return;
    }
}

int main(void) {
    IO_init();         // init OLED

    MY_BACK_COLOR = MY_BLACK;

    while(1) {
        Button0 = 0, Button1 = 0, Boot = 0;
        switch(CurState) {
            case MenuPlay: {
                memset(whole_pic, 0, sizeof(whole_pic));
                add_string(whole_pic, 60, 18, (u8*)("Dino!"), MY_GBLUE, 0);
                add_string(whole_pic, 28, 45, (u8*)("Play  Setting"), MY_GBLUE, 0);
                add_rectangle(whole_pic, 28, 45 + 16 + 2, 32, 1, MY_WHITE);
                show_pic(whole_pic);
                while(1) {
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    delay_1ms(BUTTON_DELAY);
                    if(Button0 + Button1 + Boot == 1) {
                        if(Button1) CurState = MenuSetting;
                        else if(Boot) CurState = PlayReady;
                        break;
                    }
                }
                break;
            }
            case MenuSetting: {
                memset(whole_pic, 0, sizeof(whole_pic));
                add_string(whole_pic, 60, 18, (u8*)("Dino!"), MY_GBLUE, 0);
                add_string(whole_pic, 28, 45, (u8*)("Play  Setting"), MY_GBLUE, 0);
                add_rectangle(whole_pic, 28 + 8 * 6, 45 + 16 + 2, 56, 1, MY_WHITE);
                show_pic(whole_pic);
                while(1) {
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    delay_1ms(BUTTON_DELAY);
                    if(Button0 + Button1 + Boot == 1) {
                        if(Button0) CurState = MenuPlay;
                        else if(Boot) CurState = SettingEasy;
                        break;
                    }
                }
                break;
            }
            case SettingEasy: {
                memset(whole_pic, 0, sizeof(whole_pic));
                add_string(whole_pic, 46, 18, (u8*)("Setting"), MY_GBLUE, 2);
                add_string(whole_pic, 40, 45, (u8*)("Easy  Hard"), MY_GBLUE, 0);
                add_rectangle(whole_pic, 40, 45 + 16 + 2, 32, 1, MY_WHITE);
                show_pic(whole_pic);
                while(1) {
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    delay_1ms(BUTTON_DELAY);
                    if(Button0 + Button1 + Boot == 1) {
                        if(Button1) CurState = SettingHard;
                        else if(Boot) {
                            CurState = MenuSetting;
                            Difficulty = Easy;
                        }
                        break;
                    }
                }
                break;
            }
            case SettingHard: {
                memset(whole_pic, 0, sizeof(whole_pic));
                add_string(whole_pic, 46, 18, (u8*)("Setting"), MY_GBLUE, 2);
                add_string(whole_pic, 40, 45, (u8*)("Easy  Hard"), MY_GBLUE, 0);
                add_rectangle(whole_pic, 40 + (4 + 2) * 8, 45 + 16 + 2, 32, 1, MY_WHITE);
                show_pic(whole_pic);
                while(1) {
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    delay_1ms(BUTTON_DELAY);
                    if(Button0 + Button1 + Boot == 1) {
                        if(Button0) CurState = SettingEasy;
                        else if(Boot) {
                            CurState = MenuSetting;
                            Difficulty = Hard;
                        }
                        break;
                    }
                }
                break;
            }
            case PlayReady: {
                set_difficulty(Easy);
                FRAME_SPEED = 6;
                BIRD_GENERATED_INTERVAL = 25;
                CACTUS_GENERATED_INTERVAL = 25;
                InitGame();
                frozen_frame = 0;
                while(1) {
                    AutoPlay();
                    int tmp0 = Button0;
                    int tmp1 = Button1;
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    if(Button0 + Button1 + Boot == 1) {
                        CurState = Playing;
                        break;
                    }
                    Button0 = tmp0;
                    Button1 = tmp1;
                }
                break;
            }
            case Playing: {
                set_difficulty(Difficulty);
                InitGame();
                // [0, 160) [0, 10)
                while(1) {
                    add_bird(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
                    GAME_OVER = add_dinosaur(dinosaur_tmp_pic, PRONE_DINO_WIDTH, DINO_HEIGHT);
                    show_score_and_mode();
                    if(GAME_OVER) {
                        add_string(whole_pic, 40, 22, (u8*)("Game Over!"), MY_GBLUE, 0);
                        show_pic(whole_pic);
                        CurState = GameOver;
                        break;
                    } else {
                        show_pic(whole_pic);
                        recovery_bird(bird_tmp_pic, BIRD_WIDTH, BIRD_HEIGHT);
                        recovery_dinosaur(dinosaur_tmp_pic, PRONE_DINO_WIDTH, DINO_HEIGHT);
                        shift_back_pic(whole_pic, FRAME_SPEED);
                        add_rectangle(whole_pic, BOARD_WIDTH - FRAME_SPEED, 0, FRAME_SPEED, BOARD_HEIGHT, MY_BLACK);
                        add_rectangle(whole_pic, BOARD_WIDTH - FRAME_SPEED, GROUND_TOP, FRAME_SPEED, 1, MY_WHITE);
                        frame++;
                        score += SCORE_PER_FRAME;
                        update_bird();
                        generate_obstacle();
                        get_button_input();
                        update_input_seed();
                    }
                    delay_1ms(30);
                }
                break;
            }
            case GameOver: {
                while(1) {
                    Button0 = Get_Button(0);
                    Button1 = Get_Button(1);
                    Boot = Get_BOOT0();
                    delay_1ms(BUTTON_DELAY);
                    if(Button0 + Button1 + Boot == 1) {
                        if(Button0 || Button1) CurState = PlayReady;
                        else if(Boot) CurState = MenuPlay;
                        break;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
        // LCD_Clear(BLACK);
        
        // delay_1ms(7);
    }
}
