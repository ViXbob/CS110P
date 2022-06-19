#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H 
#include "lcd/lcd.h"

typedef enum _StateMachine_State {MenuPlay, MenuSetting, SettingEasy, SettingHard, PlayReady, Playing, GameOver} GameState;

static const GameState StartState = MenuPlay;

typedef enum _Difficulty_State {Easy, Hard} GameDifficulty;

#endif