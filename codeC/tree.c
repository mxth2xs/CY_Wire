#include "tree.h"

AVLNode* createNode(int key, long capacity, double consumption) {
/**
 * @brief Creates a new AVL tree node with the given key, capacity, and consumption.
 * 
 * @param key The key associated with the new node.
 * @param capacity The capacity value of the new node.
 * @param consumption The consumption value of the new node.
 * @return A pointer to the newly created AVL tree node.
 */

    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->capacity = capacity;
    node->consumption = consumption;
    node->left = node->right = NULL;
    node->height = 1; 
    return node;
}


int getHeight(AVLNode* node) {
/**
 * @brief Retrieves the height of a given AVL tree node.
 * 
 * @param node The AVL tree node whose height is to be determined.
 * @return The height of the node, or 0 if the node is NULL.
 */

    return node ? node->height : 0;
}


int getBalance(AVLNode* node) {
/**
 * @brief Calculates the balance factor of a given AVL tree node.
 * 
 * @param node The AVL tree node for which the balance factor is calculated.
 * @return The balance factor of the node, or 0 if the node is NULL.
 */

    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}


AVLNode* rotateRight(AVLNode* y) {
/**
 * @brief Performs a right rotation on a given AVL tree node.
 * 
 * @param y The root of the subtree to rotate right.
 * @return The new root of the rotated subtree.
 */

    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + ( (getHeight(y->left) > getHeight(y->right)) ? getHeight(y->left) : getHeight(y->right) );
    x->height = 1 + ( (getHeight(x->left) > getHeight(x->right)) ? getHeight(x->left) : getHeight(x->right) );

    return x;
}


AVLNode* rotateLeft(AVLNode* x) {
/**
 * @brief Performs a left rotation on a given AVL tree node.
 * 
 * @param x The root of the subtree to rotate left.
 * @return The new root of the rotated subtree.
 */

    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + ( (getHeight(x->left) > getHeight(x->right)) ? getHeight(x->left) : getHeight(x->right) );
    y->height = 1 + ( (getHeight(y->left) > getHeight(y->right)) ? getHeight(y->left) : getHeight(y->right) );

    return y;
}


AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption) {
/**
 * @brief Inserts a node with the specified key, capacity, and consumption into the AVL tree.
 * 
 * @param root The root of the AVL tree.
 * @param key The key of the node to be inserted.
 * @param capacity The capacity value of the node to be inserted.
 * @param consumption The consumption value of the node to be inserted.
 * @return The new root of the AVL tree after the insertion.
 */

    if (!root) {
        return createNode(key, capacity, consumption);
    }

    if (key < root->key) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        // If the key already exists, just update the capacity and consumption
        //root->capacity += capacity;
        root->consumption += consumption;
        return root;
    }

    // Update height
    root->height = 1 + ((getHeight(root->left) > getHeight(root->right)) ? getHeight(root->left) : getHeight(root->right));

    // Check balance and rotate if needed
    int balance = getBalance(root);

    // Left-left case
    if (balance > 1 && key < root->left->key) {
        return rotateRight(root);
    }

    // Right-right case
    if (balance < -1 && key > root->right->key) {
        return rotateLeft(root);
    }

    // Left-right case
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right-left case
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}


AVLNode* searchNode(AVLNode* root, int key) {
/**
 * @brief Searches for a node with the specified key in the AVL tree.
 * 
 * @param root The root of the AVL tree.
 * @param key The key of the node to search for.
 * @return A pointer to the node with the specified key, or NULL if not found.
 */

    if (!root || root->key == key) {
        return root;
    }

    if (key < root->key) {
        return searchNode(root->left, key);
    } else {
        return searchNode(root->right, key);
    }
}


void freeTree(AVLNode* root) {
/**
 * @brief Frees the memory allocated for the AVL tree.
 * 
 * @param root The root of the AVL tree to be freed.
 */

    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
