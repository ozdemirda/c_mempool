CC = gcc

SOURCE_DIR = src
INCLUDE_DIR = include
OBJECT_DIR = obj

_create_object_dir := $(shell mkdir -p $(OBJECT_DIR))

CFLAGS = -I$(INCLUDE_DIR) -c -fPIC -fstack-protector-all \
	-Wstrict-overflow -Wformat=2 -Wformat-security -Wall -Wextra \
	-g3 -O3 -Werror
LFLAGS = -shared -lpthread -lm

SOURCE_FILES = $(SOURCE_DIR)/cmempool.c
HEADER_FILES = $(INCLUDE_DIR)/cmempool.h
OBJ_FILES = $(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(OBJECT_DIR)/%.o)

default: all

all: libcmempool.so

libcmempool.so: $(OBJ_FILES)
	$(CC) -o libcmempool.so $(OBJ_FILES) $(LFLAGS)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADER_FILES)
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf libcmempool.so $(OBJECT_DIR) test/tests test/coverage
