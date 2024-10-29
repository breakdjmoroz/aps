NAME=model.out
CC=gcc

INCLUDE=./include
SOURCE=./src
BINARY=./bin

all:
	gcc $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(NAME)

clean:
	rm $(BINARY)/*
