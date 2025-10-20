CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc
SRCDIR = src
DATADIR = data
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/cliente.c $(SRCDIR)/banco.c $(SRCDIR)/ordenacao.c $(SRCDIR)/io.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = banco

all: $(TARGET) $(DATADIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Compilacao concluida com sucesso!"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DATADIR):
	mkdir -p $(DATADIR)
	@echo "Pasta data/ criada"

clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Arquivos de compilacao removidos"

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run