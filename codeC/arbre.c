#include "arbre.h"

// Créer un nœud de l'arbre
AVLNode* createNode(int key, long capacity, long consumption) {
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

    return y;
}

// Insertion dans l'arbre AVL
AVLNode* insertNode(AVLNode* root, int key, long capacity, long consumption) {
    if (!root) return createNode(key, capacity, consumption);

    if (key < root->key) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        // Mise à jour si le nœud existe déjà
        root->capacity += capacity;
        root->consumption += consumption;
        return root;
    }

    // Mise à jour de la hauteur
    root->height = 1 + (getHeight(root->left) > getHeight(root->right) ? getHeight(root->left) : getHeight(root->right));

    // Vérification de l'équilibre et rotation si nécessaire
    int balance = getBalance(root);

    // Rotation gauche-gauche
    if (balance > 1 && key < root->left->key)
        return rotateRight(root);

    // Rotation droite-droite
    if (balance < -1 && key > root->right->key)
        return rotateLeft(root);

    // Rotation gauche-droite
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Rotation droite-gauche
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Recherche dans l'arbre AVL
AVLNode* searchNode(AVLNode* root, int key) {
    if (!root || root->key == key)
        return root;

    if (key < root->key)
        return searchNode(root->left, key);
    else
        return searchNode(root->right, key);
}

// Parcours In-Order de l'arbre
void inorderTraversal(AVLNode* root) {
    if (root) {
        inorderTraversal(root->left);
        printf("ID: %d, Capacity: %ld, Consumption: %ld\n", root->key, root->capacity, root->consumption);
        inorderTraversal(root->right);
    }
}

// Libérer la mémoire
void freeTree(AVLNode* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
