#include "arbre.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Créer un nœud de l'arbre
AVLNode* createNode(int key, long capacity, double consumption) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->capacity = capacity;
    node->consumption = consumption;
    node->left = node->right = NULL;
    node->height = 1; // Hauteur initiale
    return node;
}

// Obtenir la hauteur d'un nœud
int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// Calculer le facteur d'équilibre
int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Rotation droite
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));

    //printf("Rotation droite effectuée sur le nœud : %d\n", y->key);
    return x;
}

// Rotation gauche
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));

    //printf("Rotation gauche effectuée sur le nœud : %d\n", x->key);
    return y;
}

// Insertion dans l'arbre AVL
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption) {
    // Si l'arbre est vide, créer un nouveau nœud
    if (!root) {
        return createNode(key, capacity, consumption);
    }

    if (key < root->key) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        root->capacity += capacity;
        root->consumption += consumption;
        return root;
    }

    // Mise à jour de la hauteur
    root->height = 1 + (getHeight(root->left) > getHeight(root->right) ? getHeight(root->left) : getHeight(root->right));

    // Vérification de l'équilibre et rotations si nécessaires
    int balance = getBalance(root);

    if (balance > 1 && key < root->left->key) {
        return rotateRight(root);
    }

    if (balance < -1 && key > root->right->key) {
        return rotateLeft(root);
    }

    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Recherche dans l'arbre AVL
AVLNode* searchNode(AVLNode* root, int key) {
    if (!root) {
        printf("Recherche échouée : Nœud avec la clé %d introuvable (arbre vide ou nœud inexistant).\n", key);
        return NULL;
    }

    if (root->key == key) {
        printf("Nœud trouvé : Key=%d, Capacity=%ld, Consumption=%.2f\n", root->key, root->capacity, root->consumption);
        return root;
    }

    if (key < root->key) {
        return searchNode(root->left, key);
    } else {
        return searchNode(root->right, key);
    }
}

// Parcours In-Order de l'arbre
void inorderTraversal(AVLNode* root) {
    if (!root) return;

    inorderTraversal(root->left);
    printf("ID: %d, Capacity: %ld, Consumption: %.2f\n", root->key, root->capacity, root->consumption);
    inorderTraversal(root->right);
}

// Libérer la mémoire
void freeTree(AVLNode* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
