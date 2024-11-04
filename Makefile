NAME=model.out
DBG_NAME=model.dbg
CC=gcc

INCLUDE=./include
SOURCE=./src
BINARY=./bin

all:
	gcc $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(NAME)

debug:
	gcc -g $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(DBG_NAME)

clean:
	rm $(BINARY)/*

run:
	$(BINARY)/$(NAME)
