CC = g++
FLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual

front: frontend/main.cpp frontend/stack.cpp frontend/output.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
	$(CC) $(FLAGS) frontend/main.cpp frontend/output.cpp frontend/stack.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
cpu: processor/
