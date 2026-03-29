CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include -I./src
LDFLAGS = -lsqlite3

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TESTDIR = tests

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
TARGET = $(BINDIR)/autopark

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

run: $(TARGET)
	./$(TARGET)

check:
	@echo "Running tests..."
	@gcc $(TESTDIR)/test_driver.c -o $(TESTDIR)/test_driver -lsqlite3 -I./include -I./src
	@./$(TESTDIR)/test_driver

distcheck: check

.PHONY: clean run check distcheck
