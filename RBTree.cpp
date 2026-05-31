/**
 * @file RBTree.cpp
 * @brief Реализация методов красно-чёрного дерева.
 */

#include "RBTree.hpp"

// RBNode 
RBNode::RBNode(const string& k, const Flight& f)
    : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {
    values.push_back(f);
}

// RBTree 
RBTree::RBTree() : root(nullptr) {}

RBTree::~RBTree() {
    clear();
}

/**
 * @brief Левый поворот вокруг узла x.
 * @param x Узел, вокруг которого выполняется поворот.
 */
void RBTree::rotateLeft(RBNode* x) {
    RBNode* y = x->right;
    if (!y) return;

    // y занимает место x
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    // левый потомок y становится правым потомком x
    x->right = y->left;
    if (y->left != nullptr)
        y->left->parent = x;

    // x становится левым потомком y
    y->left = x;
    x->parent = y;
}

/**
 * @brief Правый поворот вокруг узла x.
 * @param x Узел, вокруг которого выполняется поворот.
 */
void RBTree::rotateRight(RBNode* x) {
    RBNode* y = x->left;
    if (!y) return;

    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    x->left = y->right;
    if (y->right != nullptr)
        y->right->parent = x;

    y->right = x;
    x->parent = y;
}

/**
 * @brief Восстановление свойств красно-чёрного дерева после вставки.
 * @param z Вставленный красный узел.
 * 
 * @details Алгоритм:
 *          - Пока родитель z красный, нарушение свойств.
 *          - В зависимости от того, является ли родитель левым или правым
 *            потомком деда, и от цвета дяди применяются перекрашивания
 *            и/или повороты.
 *          - В конце корень окрашивается в чёрный цвет.
 */
void RBTree::fixInsert(RBNode* z) {
    while (z->parent && z->parent->color == RED) {
        // Случай 1: родитель z — левый потомок деда
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right; // дядя
            if (y && y->color == RED) {
                // Случай 1: дядя красный → перекрашивание
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // поднимаемся к деду
            } else {
                // Случай 2: дядя чёрный (или nullptr)
                if (z == z->parent->right) {
                    // левый поворот
                    z = z->parent;
                    rotateLeft(z);
                }
                // затем правый поворот и перекраска
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else {
            // Симметричный случай: родитель z — правый потомок деда
            RBNode* y = z->parent->parent->left; // дядя
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->color = BLACK; // корень всегда чёрный
}

void RBTree::insert(const Flight& f) {
    const string& key = f.getKey();
    RBNode* z = new RBNode(key, f); //
    RBNode* y = nullptr; // будет родителем для z
    RBNode* x = root; // текущий узел при спуске

    // Поиск места для вставки (как в обычном BST)
    while (x != nullptr) {
        y = x;
        if (key < x->key)
            x = x->left;
        else if (key > x->key)
            x = x->right;
        else {
            // Ключ уже существует – добавляем в вектор и удаляем z
            x->values.push_back(f);
            delete z;
            return;
        }
    }

    // Вставляем z как ребёнка y
    z->parent = y;
    if (y == nullptr) {
        root = z;
    } else if (key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    // Восстанавливаем свойства красно-чёрного дерева
    fixInsert(z);
}

vector<Flight> RBTree::find(const string& key) const {
    RBNode* node = findNode(root, key);
    if (node != nullptr)
        return node->values;
    return {};
}

RBNode* RBTree::findNode(RBNode* node, const string& key) const {
    while (node != nullptr) {
        if (key < node->key)
            node = node->left;
        else if (key > node->key)
            node = node->right;
        else
            return node;
    }
    return nullptr;
}

void RBTree::clear() {
    clear(root);
    root = nullptr;
}

void RBTree::clear(RBNode* node) {
    if (node == nullptr) return;
    clear(node->left);
    clear(node->right);
    delete node;
}