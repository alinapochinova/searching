/**
 * @file BSTree.cpp
 * @brief Реализация методов бинарного дерева поиска.
 */

#include "BSTree.hpp"

BSTNode::BSTNode(const string& k, const Flight& f)
    : key(k), left(nullptr), right(nullptr) {
    values.push_back(f);
}

BSTree::BSTree() : root(nullptr) {}

BSTree::~BSTree() {
    clear();
}

void BSTree::insert(const Flight& f) {
    insert(root, f);
}

void BSTree::insert(BSTNode*& node, const Flight& f) {
    const string& key = f.getKey();
    if (node == nullptr) {
        node = new BSTNode(key, f);
        return;
    }
    if (key < node->key) {
        insert(node->left, f);
    } else if (key > node->key) {
        insert(node->right, f);
    } else {
        // ключ совпадает – добавляем в существующий вектор
        node->values.push_back(f);
    }
}

vector<Flight> BSTree::find(const string& key) const {
    BSTNode* node = findNode(root, key);
    if (node != nullptr)
        return node->values;
    return {};
}

BSTNode* BSTree::findNode(BSTNode* node, const string& key) const {
    if (node == nullptr || node->key == key)
        return node;
    if (key < node->key)
        return findNode(node->left, key);
    else
        return findNode(node->right, key);
}

void BSTree::clear() {
    clear(root);
    root = nullptr;
}

void BSTree::clear(BSTNode* node) {
    if (node == nullptr) return;
    clear(node->left);
    clear(node->right);
    delete node;
}