#ifndef ARBRE_H
#define ARBRE_H

#include <stdio.h>
#include <stdlib.h>

// Structure de nœud pour un arbre AVL
typedef struct AVLNode {
    int key;                   // Clé unique (par ex., ID de la station)
    long capacity;             // Capacité totale
    double consumption;        // Consommation totale
    struct AVLNode* left;      // Sous-arbre gauche
    struct AVLNode* right;     // Sous-arbre droit
    int height;                // Hauteur du nœud
} AVLNode;

// Prototypes des fonctions AVL
AVLNode* createNode(int key, long capacity, double consumption);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption);
AVLNode* searchNode(AVLNode* root, int key);
void inorderTraversalToCSV(AVLNode* root, FILE* outputFile); // Déclaration ajoutée
void freeTree(AVLNode* root);

#endif