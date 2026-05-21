# =============================================================================
# KINETIX CORE : BARE-METAL HFT INTERLOCK
# =============================================================================

CC = gcc
CFLAGS = -O3 -Wall -Wextra -march=native -mtune=native -fno-strict-aliasing
TARGET = bench_kinetix

SRC = bench_kinetix.c kinetix_core.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
