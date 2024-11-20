# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
SRCDIR = codeC
OBJDIR = $(SRCDIR)/obj
BINDIR = $(SRCDIR)/bin
EXEC = main

# Liste des fichiers sources et objets
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Règle par défaut
all: directories $(BINDIR)/$(EXEC)

# Création des dossiers si nécessaires
directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

# Règle pour générer l'exécutable
$(BINDIR)/$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle pour compiler les fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers générés
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Nettoyer les fichiers générés et l'exécutable
distclean: clean
	rm -f $(BINDIR)/$(EXEC)

# Aide
help:
	@echo "Options de Makefile :"
	@echo "  all        - Compile le programme."
	@echo "  clean      - Supprime les fichiers objets et les répertoires temporaires."
	@echo "  distclean  - Supprime tout, y compris l'exécutable."
	@echo "  help       - Affiche ce message d'aide."
