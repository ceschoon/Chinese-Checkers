#! /bin/bash

if [ $# -gt 0 ]
then
	if [ $1 == "debug" ]
	then
		g++ -o chinese_checkers main.cpp Board.h Board.cpp \
			-lsfml-graphics -lsfml-window -lsfml-system \
			-DDEBUG
		./chinese_checkers > out.txt 2> out2.txt
	fi
	
	if [ $1 == "tests" ]
	then
		g++ -O3 -o tests test_algorithms.cpp Board.h Board.cpp \
			-lsfml-graphics -lsfml-window -lsfml-system
		time ./tests > out.txt 2> out2.txt
		cat out.txt
		cat out2.txt
	fi
else
	g++ -o chinese_checkers main.cpp Board.h Board.cpp \
		-lsfml-graphics -lsfml-window -lsfml-system
	./chinese_checkers 
fi

