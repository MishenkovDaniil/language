CC = g++
FLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual

asm: asm_asm.o asm_main.o asm_strings.o io.o
	$(CC) asm_asm.o asm_strings.o asm_main.o io.o
	echo "Enter name of the object file (asm.o), name of input and output files to assembly"

cpu: cpu_main.o cpu.o check.o
	$(CC) $(FLAGS) cpu_main.o cpu.o check.o
	echo "Enter name of the object file (cpu.o), name of files to process"

cpu_main.o: cpu/main.cpp
	$(CC) $(FLAGS) cpu/main.cpp

cpu.o: cpu/cpu.cpp
	$(CC) $(FLAGS) cpu/cpu.cpp

check.o: cpu/check.cpp
	$(CC) $(FLAGS) cpu/check.cpp

asm_asm.o: asm/asm.cpp
	$(CC) $(FLAGS) asm/asm.cpp

asm_strings.o: asm/asm_strings.cpp
	$(CC) $(FLAGS) asm/asm_strings.cpp

asm_main.o: asm/main.cpp
	$(CC) $(FLAGS) asm/main.cpp

io.o: ../../standart_functions/io/io.cpp
	$(CC) $(FLAGS) ../../standart_functions/io/io.cpp
