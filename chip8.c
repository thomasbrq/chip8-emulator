#include "chip8.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Use: ./chip8_emulator </path/to/rom.ch8>\n");
		exit(0);
	}

	init_chip8();
	load_rom(argv[1]);

	InitWindow(SCREEN_WIDTH * SIZE_MULTIPLIER, SCREEN_HEIGHT * SIZE_MULTIPLIER, "CHIP8 Emulator by meu");

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		/**
		 * Do <OPS_PER_FRAME> operations at each frame
		*/
		for (size_t i = 0; i < OPS_PER_FRAME; i++)
			interpreter();
	
		BeginDrawing();

			ClearBackground(BLACK);

			update_display();

		EndDrawing();

		decrement_timers();
		
		WaitTime(16 / 1000);
	}

	CloseWindow();

	return (0);
}