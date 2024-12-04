NAME=model.out
DBG_NAME=model.dbg
CONF=model.conf

CC=gcc

INCLUDE=./include
SOURCE=./src
BINARY=./bin

LIB=-lc -lm

all:
	$(CC) $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(NAME) -L $(LIB)

debug:
	$(CC) -g $(wildcard $(SOURCE)/*.c) -iquote $(INCLUDE) -o $(BINARY)/$(DBG_NAME) -L $(LIB)

clean:
	rm $(BINARY)/*

run:
	$(BINARY)/$(NAME) $$(cat $(CONF) | tail -n1)
