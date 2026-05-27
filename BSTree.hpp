/**
 * @file BSTree.hpp
 * @brief Заголовочный файл бинарного дерева поиска (BST).
 * 
 * @details Реализует бинарное дерево поиска для хранения объектов Flight.
 *          Ключом является номер рейса (flightNumber). Поддерживаются дубликаты:
 *          в каждом узле хранится вектор всех объектов с одинаковым ключом.
 */

#pragma once
#include <vector>
#include <string>
#include "flight.hpp"

using namespace std;

/**
 * @struct BSTNode
 * @brief Узел бинарного дерева поиска.
 */
struct BSTNode {
    string key;                     ///< Ключ (номер рейса)
    vector<Flight> values;          ///< Все рейсы с этим ключом
    BSTNode* left;                  ///< Указатель на левое поддерево
    BSTNode* right;                 ///< Указатель на правое поддерево

    /**
     * @brief Конструктор узла.
     * @param k Ключ.
     * @param f Объект Flight (добавляется в вектор values).
     */
    BSTNode(const string& k, const Flight& f);
};

/**
 * @class BSTree
 * @brief Класс бинарного дерева поиска.
 * 
 * @details Обеспечивает:
 *          - Вставку элементов с дубликатами.
 *          - Поиск всех элементов по ключу.
 *          - Очистку дерева.
 */
class BSTree {
public:
    /** @brief Конструктор. Инициализирует пустое дерево. */
    BSTree();

    /** @brief Деструктор. Освобождает всю память дерева. */
    ~BSTree();

    /**
     * @brief Вставка объекта Flight в дерево.
     * @param f Объект для вставки.
     * @details Если ключ (flightNumber) уже существует, объект добавляется
     *          в вектор values соответствующего узла. Иначе создаётся новый узел.
     */
    void insert(const Flight& f);

    /**
     * @brief Поиск всех объектов с заданным ключом.
     * @param key Номер рейса для поиска.
     * @return Вектор объектов Flight (может быть пустым, если ключ не найден).
     */
    vector<Flight> find(const string& key) const;

    /** @brief Очистка дерева (удаление всех узлов). */
    void clear();

private:
    BSTNode* root;                  ///< Корень дерева

    /** @brief Рекурсивная вставка. */
    void insert(BSTNode*& node, const Flight& f);

    /** @brief Рекурсивный поиск узла по ключу. */
    BSTNode* findNode(BSTNode* node, const string& key) const;

    /** @brief Рекурсивная очистка поддерева. */
    void clear(BSTNode* node);
};