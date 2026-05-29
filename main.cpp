/**
 * @file main.cpp
 * @brief Программа для сравнения времени поиска в различных структурах данных.
 * 
 * @details Загружает данные об авиарейсах из CSV-файлов разного размера,
 *          строит структуры поиска (BST, красно-чёрное дерево, хэш-таблица,
 *          std::multimap) и замеряет время выполнения 10 поисковых запросов.
 *          Результаты сохраняются в CSV-файл для последующего построения графиков.
 * 
 * @see Flight, BSTree, RBTree, HashTable
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <algorithm>
#include <set>
#include "Flight.hpp"
#include "BSTree.hpp"
#include "RBTree.hpp"
#include "HashTable.hpp"

using namespace std;
using namespace chrono;

// ------------------------------------------------------------
// Вспомогательные функции
// ------------------------------------------------------------

/**
 * @brief Загрузка данных из CSV-файла.
 * @param filename Имя файла.
 * @return Вектор объектов Flight.
 */
vector<Flight> loadFromCSV(const string& filename) {
    vector<Flight> flights;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return flights;
    }
    string header;
    getline(file, header); // пропускаем заголовок
    Flight f;
    while (file >> f) {
        flights.push_back(f);
    }
    return flights;
}

/**
 * @brief Линейный поиск всех вхождений ключа в массиве.
 * @param data Вектор объектов.
 * @param key Искомый ключ (номер рейса).
 * @return Вектор найденных объектов.
 */
vector<Flight> linearSearch(const vector<Flight>& data, const string& key) {
    vector<Flight> result;
    for (const auto& f : data) {
        if (f.getKey() == key)
            result.push_back(f);
    }
    return result;
}

/**
 * @brief Получение первых N уникальных ключей из массива.
 * @param data Вектор объектов.
 * @param count Желаемое количество уникальных ключей.
 * @return Вектор уникальных ключей (не более count).
 */
vector<string> getUniqueKeys(const vector<Flight>& data, size_t count) {
    set<string> unique;
    for (const auto& f : data) {
        unique.insert(f.getKey());
        if (unique.size() >= count) break;
    }
    return vector<string>(unique.begin(), unique.end());
}

// ------------------------------------------------------------
// Главная функция
// ------------------------------------------------------------

int main() {
    // Размеры массивов (должны соответствовать сгенерированным файлам)
    vector<size_t> sizes = {
        100, 500, 1000, 5000, 10000, 20000, 30000, 50000,
        75000, 100000, 200000, 300000, 500000, 750000, 1000000
    };

    // Фиксированный размер хэш-таблицы (простое число)
    const size_t HASH_TABLE_SIZE = 200003;

    // Открываем CSV-файл для записи результатов
    ofstream csv("search_times.csv");
    if (!csv.is_open()) {
        cerr << "Ошибка: не удалось создать CSV-файл" << endl;
        return 1;
    }
    csv << "size,linear_us,bst_us,rbt_us,hash_us,multimap_us,collisions\n";

    cout << "Начало бенчмарка..." << endl;

    for (size_t n : sizes) {
        cout << "Обработка n = " << n << "..." << endl;

        // 1. Загрузка данных
        string filename = "data/flights_" + to_string(n) + ".csv";
        vector<Flight> data = loadFromCSV(filename);
        if (data.empty()) {
            cerr << "Предупреждение: нет данных для n = " << n << ", пропускаем" << endl;
            continue;
        }

        // 2. Выбор ключей для поиска (10 уникальных)
        vector<string> keys = getUniqueKeys(data, 10);
        if (keys.empty()) {
            cerr << "Предупреждение: не удалось получить ключи для n = " << n << endl;
            continue;
        }

        // 3. Построение структур поиска

        // BST
        BSTree bst;
        for (const auto& f : data) bst.insert(f);

        // RBTree
        RBTree rbt;
        for (const auto& f : data) rbt.insert(f);

        // HashTable (фиксированный размер)
        HashTable ht(HASH_TABLE_SIZE);
        for (const auto& f : data) ht.insert(f);
        size_t collisions = ht.getCollisions();

        // std::multimap
        multimap<string, Flight> mmap;
        for (const auto& f : data) mmap.insert({f.getKey(), f});

        // 4. Замеры времени (каждый замер выполняется один раз для всех ключей,
        //    затем делим на количество ключей, чтобы получить среднее время одного поиска)

        // Линейный поиск
        auto start = high_resolution_clock::now();
        for (const string& key : keys) {
            linearSearch(data, key);
        }
        auto end = high_resolution_clock::now();
        double linear_time = duration_cast<microseconds>(end - start).count() / (double)keys.size();

        // BST поиск
        start = high_resolution_clock::now();
        for (const string& key : keys) {
            bst.find(key);
        }
        end = high_resolution_clock::now();
        double bst_time = duration_cast<microseconds>(end - start).count() / (double)keys.size();

        // RBTree поиск
        start = high_resolution_clock::now();
        for (const string& key : keys) {
            rbt.find(key);
        }
        end = high_resolution_clock::now();
        double rbt_time = duration_cast<microseconds>(end - start).count() / (double)keys.size();

        // Хэш-таблица поиск
        start = high_resolution_clock::now();
        for (const string& key : keys) {
            ht.find(key);
        }
        end = high_resolution_clock::now();
        double hash_time = duration_cast<microseconds>(end - start).count() / (double)keys.size();

        // std::multimap поиск
        start = high_resolution_clock::now();
        for (const string& key : keys) {
            auto range = mmap.equal_range(key);
            (void)range; // подавляем предупреждение о неиспользуемой переменной
        }
        end = high_resolution_clock::now();
        double mmap_time = duration_cast<microseconds>(end - start).count() / (double)keys.size();

        // 5. Запись в CSV
        csv << n << ","
            << linear_time << ","
            << bst_time << ","
            << rbt_time << ","
            << hash_time << ","
            << mmap_time << ","
            << collisions << "\n";

        cout << "  Линейный: " << linear_time << " мкс, "
             << "BST: " << bst_time << " мкс, "
             << "RBT: " << rbt_time << " мкс, "
             << "Hash: " << hash_time << " мкс, "
             << "multimap: " << mmap_time << " мкс, "
             << "коллизий: " << collisions << endl;
    }

    csv.close();
    cout << "Бенчмарк завершён. Результаты сохранены в search_times.csv" << endl;
    return 0;
}