/**
 * @file RBTree.hpp
 * @brief Заголовочный файл красно-чёрного дерева (Red-Black Tree).
 * 
 * @details Реализует самобалансирующееся двоичное дерево поиска с
 *          поддержкой дубликатов. Каждый узел хранит ключ (номер рейса)
 *          и вектор объектов Flight с этим ключом.
 */

#pragma once
#include <vector>
#include <string>
#include "Flight.hpp"

using namespace std;

/**
 * @enum Color
 * @brief Цвет узла красно-чёрного дерева.
 */
enum Color { RED, BLACK };

/**
 * @struct RBNode
 * @brief Узел красно-чёрного дерева.
 */
struct RBNode {
    string key;                     ///< Ключ (номер рейса)
    vector<Flight> values;          ///< Все рейсы с этим ключом
    Color color;                    ///< Цвет узла
    RBNode* left;                   ///< Указатель на левое поддерево
    RBNode* right;                  ///< Указатель на правое поддерево
    RBNode* parent;                 ///< Указатель на родительский узел

    /**
     * @brief Конструктор узла.
     * @param k Ключ.
     * @param f Объект Flight (добавляется в вектор values).
     * @details Новый узел всегда создаётся красным.
     */
    RBNode(const string& k, const Flight& f);
};

/**
 * @class RBTree
 * @brief Класс красно-чёрного дерева.
 * 
 * @details Обеспечивает логарифмическое время вставки и поиска
 *          благодаря балансировке. Поддерживает дубликаты ключей.
 */
class RBTree {
public:
    /** @brief Конструктор. Инициализирует пустое дерево. */
    RBTree();

    /** @brief Деструктор. Освобождает память дерева. */
    ~RBTree();

    /**
     * @brief Вставка объекта Flight в дерево.
     * @param f Объект для вставки.
     * @details Если ключ уже существует, объект добавляется в вектор
     *          существующего узла. Иначе создаётся новый красный узел,
     *          после чего выполняются повороты и перекрашивания для
     *          восстановления свойств красно-чёрного дерева.
     */
    void insert(const Flight& f);

    /**
     * @brief Поиск всех объектов с заданным ключом.
     * @param key Номер рейса.
     * @return Вектор объектов Flight (пустой, если ключ не найден).
     */
    vector<Flight> find(const string& key) const;

    /** @brief Очистка дерева (удаление всех узлов). */
    void clear();

private:
    RBNode* root;                   ///< Корень дерева
    void rotateLeft(RBNode* x);
    void rotateRight(RBNode* x);
    void fixInsert(RBNode* z);
    RBNode* findNode(RBNode* node, const string& key) const;
    void clear(RBNode* node);
};