CC = g++
FLAGS = -Wall -Wextra -g
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

main.exe : main.cpp
	$(CC) main.cpp -c main.o  $(FLAGS)
	$(CC) main.cpp -o main.exe $(LIBS) $(FLAGS)
