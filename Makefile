TARGET := hoopoe 
BUILD_DIR := ./build

SRCS := src/options.c src/main.c
SRCS += src/shared/packets.c src/shared/connection_info.c src/shared/time.c
SRCS += src/client/client.c src/client/packet_handler.c src/client/ui.c
SRCS += src/server/server.c src/server/packet_handler.c src/server/pollfd_manager.c src/server/user.c
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
