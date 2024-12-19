# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
OBJDIR = obj
BINDIR = bin
EXEC = main

MAKEFLAGS += --no-print-directory

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

all: directories $(BINDIR)/$(EXEC)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

$(BINDIR)/$(EXEC): $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

distclean: clean
	@rm -rf $(BINDIR)
