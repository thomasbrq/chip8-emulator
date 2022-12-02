NAME	=	chip8_emulator

FLAGS	=	-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRCS	=	chip8_utils.c chip8.c

all: 
	cc $(SRCS) $(FLAGS) -o $(NAME)

clean:
	rm $(NAME)