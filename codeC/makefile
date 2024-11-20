# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
OBJDIR = obj
BINDIR = bin
EXEC = main

# Pour désactiver les message Entering directory et Leaving directory
MAKEFLAGS += --no-print-directory

# Détecter automatiquement les fichiers source et objets
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

# Règle par défaut
all: directories $(BINDIR)/$(EXEC)

# Création des dossiers si nécessaires
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Compilation de l'exécutable
$(BINDIR)/$(EXEC): $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^

# Compilation des fichiers objets
$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers temporaires
clean:
	@rm -rf $(OBJDIR)

# Nettoyage complet (inclut l'exécutable)
distclean: clean
	@rm -rf $(BINDIR)
