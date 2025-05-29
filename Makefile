CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
BIN = $(BIN_DIR)/craven_caverns

.PHONY: all clean run copy_assets

all: create_dirs $(BIN) copy_assets

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

create_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p assets/models
	mkdir -p assets/textures
	mkdir -p assets/sounds

copy_assets:
	bash copy_assets.sh

run: all
	./$(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN)
