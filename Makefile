CC = g++
FLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual

front: frontend/main.cpp frontend/check_input.cpp frontend/stack.cpp frontend/output.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
	$(CC) frontend/main.cpp frontend/check_input.cpp frontend/output.cpp frontend/stack.cpp frontend/lexems.cpp frontend/tree.cpp frontend/input.cpp ../standart_functions/io/io.cpp
	echo "Enter programm file, tree file and out file"

asm: backend/asm/asm.cpp backend/asm/main.cpp backend/asm/asm_strings.cpp ../standart_functions/io/io.cpp
	$(CC) backend/asm/asm.cpp backend/asm/main.cpp backend/asm/asm_strings.cpp ../standart_functions/io/io.cpp
	echo "Enter asm file and out file"

cpu: backend/cpu/main.cpp backend/cpu/cpu.cpp backend/cpu/check.cpp
	$(CC) backend/cpu/main.cpp backend/cpu/cpu.cpp backend/cpu/check.cpp
	echo "Enter file to process"
