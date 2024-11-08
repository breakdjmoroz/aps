NAME=model.out
DBG_NAME=model.dbg
CC=gcc

INCLUDE=./include
SOURCE=./src
BINARY=./bin

LIB=-lc -lm

all:
	gcc $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(NAME) -L $(LIB)

debug:
	gcc -g $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(DBG_NAME) -L $(LIB)

clean:
	rm $(BINARY)/*

run:
	$(BINARY)/$(NAME)
