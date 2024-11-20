# Nom de l'exécutable
TARGET = main

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -std=c99 -g

# Fichiers sources
SRCS = main.c arbre.c

# Fichiers objets (générés automatiquement à partir des sources)
OBJS = $(SRCS:.c=.o)

# Règle par défaut
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(OBJS) $(TARGET)

# Nettoyer tout sauf les fichiers sources
distclean: clean
	rm -f *~

# Exécution (optionnel, pour tester rapidement)
run: $(TARGET)
	./$(TARGET)
