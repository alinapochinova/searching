/**
 * @file HashTable.hpp
 * @brief Заголовочный файл хэш-таблицы с методом цепочек.
 * 
 * @details Реализует хэш-таблицу, где каждая корзина является
 *          unordered_map<key, vector<Flight>>. Это позволяет хранить
 *          дубликаты и корректно подсчитывать коллизии (только для
 *          разных ключей, попавших в одну корзину).
 */

#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Flight.hpp"

using namespace std;

/**
 * @class HashTable
 * @brief Хэш-таблица для хранения и поиска объектов Flight по ключу.
 */
class HashTable {
public:
    /**
     * @brief Конструктор.
     * @param table_size Размер таблицы (количество корзин).
     *                   Рекомендуется выбирать простое число.
     */
    explicit HashTable(size_t table_size = 200003);

    /** @brief Деструктор. */
    ~HashTable();

    /**
     * @brief Вставка объекта в таблицу.
     * @param f Объект Flight.
     * @details Если ключ (номер рейса) уже существует в соответствующей
     *          корзине, объект добавляется в существующий вектор.
     *          Если ключ отсутствует, создаётся новая запись.
     *          Коллизия увеличивается, если при вставке нового уникального
     *          ключа корзина уже была непуста.
     */
    void insert(const Flight& f);

    /**
     * @brief Поиск всех объектов по ключу.
     * @param key Номер рейса.
     * @return Вектор копий найденных объектов (пустой, если не найдены).
     */
    vector<Flight> find(const string& key) const;

    /** @return Общее количество коллизий (конфликтов разных ключей). */
    size_t getCollisions() const { return collisions; }

    /** @brief Очистка таблицы (удаление всех элементов и сброс счётчика коллизий). */
    void clear();

private:
    /**
     * @brief Хэш-функция для строки.
     * @param key Входная строка.
     * @return Индекс корзины в диапазоне [0, tableSize-1].
     */
    size_t hash(const string& key) const;

    size_t tableSize;                                 ///< Количество корзин
    vector<unordered_map<string, vector<Flight>>> buckets; ///< Корзины
    size_t collisions;                                ///< Счётчик коллизий
};