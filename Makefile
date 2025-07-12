TARGET := hoopoe 
BUILD_DIR := ./build

SRCS := $(wildcard src/*.c src/**/*.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INCLUDE = .

CFLAGS := $(shell pkg-config --cflags ncurses) $(addprefix -I,$(INCLUDE)) -ggdb -Wall -Wextra -O3 -fsanitize=address
LDFLAGS := $(shell pkg-config --libs ncurses) -lm -fsanitize=address

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
