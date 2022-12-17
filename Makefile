CC = g++
FLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual

all: frontend/main.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
	$(CC) frontend/main.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
