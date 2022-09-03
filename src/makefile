# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation
# ****************************************************
# Targets needed to bring the executable up to date

#// On linux compile with:
#// g++ -std=c++17 main.cpp -o prog -lSDL2 
#
chip_8: main.o chip-8.o  #cpu.o

		g++  main.o chip-8.o -lSDL2

main.o: main.cpp
		g++ -c main.cpp

chip-8.o: chip-8.cpp chip-8.h
		g++ -c  chip-8.cpp 

#cpu.o: cpu.cpp cpu.h
#		g++ -c cpu.cpp
		


