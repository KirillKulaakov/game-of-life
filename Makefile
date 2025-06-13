CC=gcc
FLAGS=-Wall -Werror -Wextra
ADDFLAGS=-lncurses

all: build

build: game_of_life

game_of_life: game_of_life.c
	${CC} ${FLAGS} game_of_life.c -o  game_of_life ${ADDFLAGS}

clean:
	rm -rf *.o game_of_life
