CC = gcc

PROJECT_CPPFLAGS := -Iinclude -Isrc
PROJECT_CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -O2

BUILD_DIR := build
TARGET_NAME := banco
SOURCES := \
	src/app/main.c \
	src/application/banco.c \
	src/domain/dinheiro.c \
	src/domain/ordenacao.c \
	src/infrastructure/persistencia.c \
	src/presentation/io.c
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDENCIES := $(OBJECTS:.o=.d)

ifeq ($(OS),Windows_NT)
EXE_EXT := .exe
ifneq ($(filter sh sh.exe bash bash.exe,$(notdir $(SHELL))),)
MKDIR = mkdir -p "$(1)"
RMDIR = rm -rf "$(BUILD_DIR)"
RUN = ./$(TARGET)
else
MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RMDIR = if exist "$(subst /,\,$(BUILD_DIR))" rmdir /S /Q "$(subst /,\,$(BUILD_DIR))"
RUN = $(subst /,\,$(TARGET))
endif
else
EXE_EXT :=
MKDIR = mkdir -p "$(1)"
RMDIR = rm -rf "$(BUILD_DIR)"
RUN = ./$(TARGET)
endif

TARGET := $(BUILD_DIR)/$(TARGET_NAME)$(EXE_EXT)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(call MKDIR,$(@D))
	$(CC) $(PROJECT_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: src/%.c
	@$(call MKDIR,$(@D))
	$(CC) $(PROJECT_CPPFLAGS) $(CPPFLAGS) $(PROJECT_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: all
	$(RUN)

clean:
	@$(RMDIR)

-include $(DEPENDENCIES)
