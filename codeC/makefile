# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
OBJDIR = obj
BINDIR = bin
EXEC = main

# To disable the Entering directory and Leaving directory messages
MAKEFLAGS += --no-print-directory

# Automatically detect source and object files
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

# Default rule
all: directories $(BINDIR)/$(EXEC)

# Creation of folders if necessary
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Compiling the executable
$(BINDIR)/$(EXEC): $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^

# Compiling object files
$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Cleaning temporary files
clean:
	@rm -rf $(OBJDIR)

# Full cleanup (includes executable)
distclean: clean
	@rm -rf $(BINDIR)
