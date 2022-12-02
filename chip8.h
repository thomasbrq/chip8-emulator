#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define SIZE_MULTIPLIER 5

#define MEMORY_SIZE	4096
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define FONTSET_SIZE	128

#define OPS_PER_FRAME 15

enum CHIP_COLORS {
	CHIP_BLACK,
	CHIP_WHITE
};

typedef struct	CHIP8 {
	uint8_t		memory[MEMORY_SIZE];
	uint8_t		registers[16];
	uint16_t	stack[16];

	uint16_t	I;
	uint8_t		jmp_counter;
	uint8_t		game_timer;
	uint8_t		sound_timer;

	uint16_t	pc;

	uint8_t		display[SCREEN_HEIGHT][SCREEN_WIDTH];
}				CHIP8;

CHIP8	chip8;

void		init_chip8();
void		update_display();
void		init_display();
void 		load_rom(char *path);

uint16_t	get_opcode();
void		interpreter();
void		decrement_timers();

#endif