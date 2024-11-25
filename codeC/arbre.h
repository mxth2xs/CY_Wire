#ifndef ARBRE_H
#define ARBRE_H

#include <stdio.h>
#include <stdlib.h>

// Node structure for an AVL tree
typedef struct AVLNode {
    int key;                   // Unique key (eg. station ID)
    long capacity;             // Total capacity
    double consumption;        // Total consumption
    struct AVLNode* left;      // Left subtree
    struct AVLNode* right;     // Right subtree
    int height;                // Node height
} AVLNode;

// AVL Function Prototypes
AVLNode* createNode(int key, long capacity, double consumption);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption);
AVLNode* searchNode(AVLNode* root, int key);
void inorderTraversalToCSV(AVLNode* root, FILE* outputFile); // Added statement
void freeTree(AVLNode* root);

#endif
