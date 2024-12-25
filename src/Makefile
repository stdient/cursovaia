CC := clang
FLAGS := -Wall -Wextra -lm

SRC_DIR := .
SRC := $(wildcard $(SRC_DIR)/*.c)

INC_DIR := ../include
BIN_DIR := ../bin

TARGET := main.exe

all:
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(SRC) -I$(INC_DIR) -o $(BIN_DIR)/main.exe

run:
	$(BIN_DIR)/$(TARGET)

val:
	valgrind $(BIN_DIR)/$(TARGET)