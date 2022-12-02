#include "chip8.h"

const unsigned char chip8_fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

/**
 * Get the keyboard keycodes.
*/
const int chip8_key[16] = {
	KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE,
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F
};

void	init_chip8()
{
	memset(&chip8.memory, 0, MEMORY_SIZE); // memory
	memset(&chip8.registers, 0, 16);
	memset(&chip8.stack, 0, 16);

	chip8.I = 0;
	chip8.jmp_counter = 0;
	chip8.game_timer = 0;
	chip8.sound_timer = 0;

	chip8.pc = 0x200;

	for (int i = 0; i < FONTSET_SIZE; i++)
		chip8.memory[i] = chip8_fontset[i];

	init_display();
}

void	load_rom(char *path)
{
	FILE *	file;

	file = fopen(path, "rb");
	if (file == NULL)
	{
		printf("The file doesn't exists.\n");
		exit(0);
	}

	fread(&chip8.memory[512], sizeof(uint8_t), (MEMORY_SIZE - 512), file);

	fclose(file);
}

void	decrement_timers()
{
	if (chip8.game_timer > 0)
		chip8.game_timer--;

	if (chip8.sound_timer > 0)
		chip8.sound_timer--;
}

void	init_display()
{
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++)
		for (uint8_t x = 0; x < SCREEN_WIDTH ; x++)
			chip8.display[y][x] = CHIP_BLACK;
}

void	update_display()
{
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++)
		for (uint8_t x = 0; x < SCREEN_WIDTH ; x++)
			if (chip8.display[y][x] == CHIP_WHITE)
				DrawRectangle((x * SIZE_MULTIPLIER), (y * SIZE_MULTIPLIER), SIZE_MULTIPLIER, SIZE_MULTIPLIER, WHITE);
}

uint16_t	get_opcode()
{
	return (chip8.memory[chip8.pc] << 8) + chip8.memory[chip8.pc + 1];
}

void	interpreter()
{
	uint16_t	opcode = get_opcode();

	switch (opcode & 0xF000)
	{
		uint8_t		vx;
		uint8_t		vy;

		uint8_t		key;

		uint8_t		n;
		uint8_t		pixel;

		uint8_t	reg;

		case 0x0000:
			switch (opcode & 0x00FF)
			{
				case 0x00E0:
					init_display();
					chip8.pc += 2;
					break;

				case 0x00EE:
					chip8.pc = chip8.stack[chip8.jmp_counter--];
					break;
				default:
					break;
			}
			break;
		case 0x1000:
			chip8.pc = (opcode & 0x0FFF);
			break;
		case 0x2000:
			chip8.jmp_counter++;
			chip8.stack[chip8.jmp_counter] = chip8.pc + 2;
			chip8.pc = (opcode & 0x0FFF);
			break;
		case 0x3000:
			if (chip8.registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				chip8.pc += 4;
			else
				chip8.pc += 2;
			break;
		case 0x4000:
			if (chip8.registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				chip8.pc += 4;
			else
				chip8.pc += 2;
			break;
		case 0x5000:
			if (chip8.registers[(opcode & 0x0F00) >> 8] == chip8.registers[(opcode & 0x00F0) >> 4])
				chip8.pc += 4;
			else
				chip8.pc += 2;
			break;
		case 0x6000:
			chip8.registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			chip8.pc += 2;
			break;
		case 0x7000:
			chip8.registers[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			chip8.pc += 2;
			break;
		case 0x8000:
			switch ((opcode & 0x000F))
			{
				case 0x0000:
					chip8.registers[(opcode & 0x0F00) >> 8] = chip8.registers[(opcode & 0x00F0) >> 4];
					chip8.pc += 2;
					break;
				case 0x0001:
					chip8.registers[(opcode & 0x0F00) >> 8] = (chip8.registers[(opcode & 0x0F00) >> 8] | chip8.registers[(opcode & 0x00F0) >> 4]);
					chip8.pc += 2;
					break;
				case 0x0002:
					chip8.registers[(opcode & 0x0F00) >> 8] = (chip8.registers[(opcode & 0x0F00) >> 8] & chip8.registers[(opcode & 0x00F0) >> 4]);
					chip8.pc += 2;
					break;
				case 0x0003:
					chip8.registers[(opcode & 0x0F00) >> 8] = (chip8.registers[(opcode & 0x0F00) >> 8] ^ chip8.registers[(opcode & 0x00F0) >> 4]);
					chip8.pc += 2;
					break;
				case 0x0004:
					chip8.registers[(opcode & 0x0F00) >> 8] += chip8.registers[(opcode & 0x00F0) >> 4];
					if (chip8.registers[(opcode & 0x0F00) >> 8] > 255)
						chip8.registers[0xf] = 1;
					else
						chip8.registers[0xf] = 0;
					chip8.pc += 2;
					break;
				case 0x0005:
					if (chip8.registers[(opcode & 0x0F00) >> 8] > chip8.registers[(opcode & 0x00F0) >> 4])
						chip8.registers[0xf] = 1;
					else
						chip8.registers[0xf] = 0;
					chip8.registers[(opcode & 0x0F00) >> 8] -= chip8.registers[(opcode & 0x00F0) >> 4];
					chip8.pc += 2;
					break;
				case 0x0006:
					vx = chip8.registers[(opcode & 0x0F00) >> 8];
					if ((vx & 0x000F) == 1)
						chip8.registers[0xf] = 1;
					else
						chip8.registers[0xf] = 0;
					chip8.registers[vx] = chip8.registers[vx] >> 1;
					chip8.pc += 2;
					break;
				case 0x0007:
					if (chip8.registers[(opcode & 0x00F0) >> 4] > chip8.registers[(opcode & 0x0F00) >> 8])
						chip8.registers[0xf] = 1;
					else
						chip8.registers[0xf] = 0;
					chip8.registers[(opcode & 0x0F00) >> 8] = chip8.registers[(opcode & 0x00F0) >> 4] - chip8.registers[(opcode & 0x0F00) >> 8];
					chip8.pc += 2;
					break;
				case 0x000E:
					chip8.registers[0xf] = (chip8.registers[(opcode & 0x0F00) >> 8] >> 7);
					chip8.registers[(opcode && 0x0F00) >> 8] = (chip8.registers[(opcode && 0x0F00) >> 8] << 1);
					chip8.pc += 2;
					break;
				default:
					break;
			}
			break;
		case 0x9000:
			if (chip8.registers[(opcode & 0x0F00) >> 8] != chip8.registers[(opcode & 0x00F0) >> 4])
				chip8.pc += 4;
			else
				chip8.pc += 2;
			break;
		case 0xA000:
			chip8.I = (opcode & 0x0FFF);
			chip8.pc += 2;
			break;
		case 0xB000:
			chip8.pc = ((opcode & 0x0FFF) + chip8.registers[0]);
			break;
		case 0xC000:
			chip8.registers[(opcode & 0x0F00) >> 8] = ((rand() % 256) & (opcode & 0x00FF));
			chip8.pc += 2;
			break;
		case 0xD000:
			vx = chip8.registers[(opcode & 0x0F00) >> 8];
			vy = chip8.registers[(opcode & 0x00F0) >> 4];
			n = (opcode & 0x000F);

			chip8.registers[0xF] = 0;
			for (int y = 0; y < n; y++)
			{
				pixel = chip8.memory[chip8.I + y];
				for (int x = 0; x < 8; x++)
				{
					if ((pixel & (0x80 >> x)) != 0)
					{
						if (chip8.display[vy + y][vx + x])
							chip8.registers[0xf] = 1;
						chip8.display[vy + y][vx + x] ^= 1;
					}
				}
			}
			chip8.pc += 2;
			break;
		case 0xE000:
			switch ((opcode & 0x00FF))
			{
				case 0x009E:
					key = chip8.registers[(opcode & 0x0F00) >> 8];
					if (IsKeyDown(chip8_key[key]))
						chip8.pc += 4;
					else
						chip8.pc += 2;
					break;
				case 0x00A1:
					key = chip8.registers[(opcode & 0x0F00) >> 8];
					if (IsKeyUp(chip8_key[key]))
							chip8.pc += 4;
					else
						chip8.pc += 2;
					break;
				default:
					break;
			}
			break;
		case 0xF000:
			switch ((opcode & 0x00FF))
			{
				case 0x001E:
					chip8.I += chip8.registers[(opcode & 0x0F00) >> 8];
					chip8.pc += 2;
					break;
				case 0x0029:
					chip8.I = chip8.registers[(opcode & 0x0F00) >> 8] * 0x5;
					chip8.pc += 2;
					break;
				case 0x0007:
					chip8.registers[(opcode & 0x0F00) >> 8] = chip8.game_timer;
					chip8.pc += 2;
					break;
				case 0x000A:
					for (uint8_t i = 0; i < 16; i++)
					{
						if (IsKeyPressed(chip8_key[i]))
						{
							chip8.registers[(opcode & 0x0F00) >> 8] = chip8_key[i];
							chip8.pc += 2;
							break ;
						}
					}
					break;
				case 0x0015:
					vx = (opcode & 0x0F00) >> 8;
					chip8.game_timer = vx;
					chip8.pc += 2;
					break;
				case 0x0018:
					vx = (opcode & 0x0F00) >> 8;
					chip8.sound_timer = vx;
					chip8.pc += 2;
					break;
				case 0x0033:
					chip8.memory[chip8.I] = chip8.registers[(opcode & 0x0F00) >> 8] / 100;
					chip8.memory[chip8.I + 1] = (chip8.registers[(opcode & 0x0F00) >> 8] / 10) % 10;
					chip8.memory[chip8.I + 2] = (chip8.registers[(opcode & 0x0F00) >> 8] % 100) % 10;
					chip8.pc += 2;
					break;
				case 0x0055:
					vx = (opcode & 0x0F00) >> 8;
					for (uint8_t i = 0; i <= vx; i++)
						chip8.memory[chip8.I + i] = chip8.registers[i];
					chip8.pc += 2;
					break;
				case 0x0065:
					vx = (opcode & 0x0F00) >> 8;
					for (uint8_t i = 0; i <= vx; i++)
						chip8.registers[i] = chip8.memory[chip8.I + i];
					chip8.pc += 2;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}