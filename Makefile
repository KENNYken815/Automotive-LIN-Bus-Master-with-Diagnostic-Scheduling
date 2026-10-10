CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude
BUILD := build
LIB := $(BUILD)/liblin.a
DEMO := $(BUILD)/lin_demo
TEST := $(BUILD)/lin_tests
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,$(BUILD)/%.o,$(SRC))
.PHONY: all demo test clean
all: demo test
$(BUILD):
	mkdir -p $(BUILD)
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
$(LIB): $(OBJ)
	ar rcs $@ $^
$(DEMO): examples/lin_demo.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIB) -o $@
$(TEST): tests/test_lin.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIB) -o $@
demo: $(DEMO)
	./$(DEMO)
test: $(TEST)
	./$(TEST)
clean:
	rm -rf $(BUILD)
