CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

SRC_DIR = src

SRC = main.c \
      $(SRC_DIR)/banco.c \
      $(SRC_DIR)/persistencia.c \
      $(SRC_DIR)/ordenacao.c \
      $(SRC_DIR)/io.c

OBJ = $(SRC:.c=.o)

TARGET = banco

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	del /Q $(OBJ) $(TARGET).exe
