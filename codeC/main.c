#include <stdio.h>
#include <stdlib.h>

int main() {
    // Nom du fichier CSV
    const char *nomFichier = "fichier.csv";

    // Ouvrir le fichier en mode lecture
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    // Buffer pour stocker chaque ligne (taille maximale fixée ici à 1024 caractères)
    char ligne[1024];

    // Lire chaque ligne jusqu'à la fin du fichier
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Ici, `ligne` contient la ligne lue, incluant le caractère '\n' à la fin
        printf("Ligne lue : %s", ligne);

        // Si vous devez traiter les colonnes (par exemple, séparées par des virgules)
        char *colonne = strtok(ligne, ",");
        while (colonne != NULL) {
            printf("Colonne : %s\n", colonne);
            colonne = strtok(NULL, ",");
        }

        printf("\n");
    }

    // Fermer le fichier
    fclose(fichier);

    return EXIT_SUCCESS;
}
