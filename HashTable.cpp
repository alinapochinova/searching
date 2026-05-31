/**
 * @file HashTable.cpp
 * @brief Реализация хэш-таблицы.
 */

#include "HashTable.hpp"

HashTable::HashTable(size_t table_size)
    : tableSize(table_size), buckets(table_size), collisions(0) {}

HashTable::~HashTable() {}

size_t HashTable::hash(const string& key) const {
    const size_t p = 131;         
    size_t h = 0;
    for (char c : key) {
        h = (h * p + static_cast<size_t>(c)) % tableSize;
    }
    return h;
}

void HashTable::insert(const Flight& f) {
    size_t idx = hash(f.getKey());
    auto& bucket = buckets[idx];
    auto it = bucket.find(f.getKey());

    if (it == bucket.end()) {
        // Ключ отсутствует в этой корзине – создаём новую запись
        bucket.emplace(f.getKey(), vector<Flight>{f});
        // Если корзина уже содержала другие ключи, это коллизия
        if (bucket.size() > 1) collisions++;
    } else {
        // Ключ уже есть – добавляем в существующий вектор (дубликат)
        it->second.push_back(f);
        // Коллизия не увеличивается – это дубликат, а не конфликт разных ключей
    }
}

vector<Flight> HashTable::find(const string& key) const {
    size_t idx = hash(key);
    const auto& bucket = buckets[idx];
    auto it = bucket.find(key);
    if (it != bucket.end()) {
        return it->second;
    }
    return {};
}

void HashTable::clear() {
    for (auto& bucket : buckets) bucket.clear();
    collisions = 0;
}