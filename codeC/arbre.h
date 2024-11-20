#ifndef arbre_H
#define arbre_H

#include <stdio.h>
#include <stdlib.h>

// Structure de nœud pour un arbre AVL
typedef struct AVLNode {
    int key;                   // Clé unique (par ex., ID de la station)
    long capacity;             // Capacité totale
    long consumption;          // Consommation totale
    struct AVLNode* left;      // Sous-arbre gauche
    struct AVLNode* right;     // Sous-arbre droit
    int height;                // Hauteur du nœud
} AVLNode;

// Prototypes des fonctions AVL
AVLNode* createNode(int key, long capacity, long consumption);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertNode(AVLNode* root, int key, long capacity, long consumption);
AVLNode* searchNode(AVLNode* root, int key);
void inorderTraversal(AVLNode* root);
void freeTree(AVLNode* root);

#endif
