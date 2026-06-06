/**
 * @file benchmark_hash_functions.cpp
 * @brief Сравнение различных хэш-функций для строк.
 * 
 * @details Сравниваются 4 хэш-функции: POLY (наша текущая), RS, JS, DJB.
 *          ROT13 исключён из-за аномально высокого количества коллизий (~15000),
 *          что делает его непригодным для хэш-таблиц.
 * 
 * @author Починова Алина
 * @date 2026
 */

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <random>
#include <set>
#include <functional>
#include <iomanip>
#include "Flight.hpp"
#include "HashTable.hpp"

using namespace std;
using namespace chrono;

const size_t TOTAL_SIZE = 500000;          ///< Фиксированный размер массива
const size_t UNIQUE_KEYS_COUNT = 2000;     ///< Количество уникальных ключей
const size_t HASH_TABLE_SIZE = 200003;     ///< Фиксированный размер таблицы
const int NUM_SEARCHES = 100;              ///< Количество поисков для замера
const unsigned int RANDOM_SEED = 42;       ///< Фиксированный seed для воспроизводимости

/**
 * @brief Полиномиальная хэш-функция (используется в HashTable по умолчанию).
 * @param key Входная строка.
 * @param mod Размер таблицы.
 * @return Хэш-код в диапазоне [0, mod-1].
 */
size_t polyHash(const string& key, size_t mod) {
    const size_t p = 131;
    size_t h = 0;
    for (char c : key) {
        h = (h * p + static_cast<size_t>(c)) % mod;
    }
    return h;
}

/**
 * @brief RS хэш-функция (из книги Роберта Седжвика).
 * @param key Входная строка.
 * @param mod Размер таблицы.
 * @return Хэш-код в диапазоне [0, mod-1].
 */
size_t rsHash(const string& key, size_t mod) {
    size_t a = 63689;
    const size_t b = 378551;
    size_t hash = 0;
    for (char c : key) {
        hash = (hash * a + static_cast<size_t>(c)) % mod;
        a = a * b;
    }
    return hash % mod;
}

/**
 * @brief JS хэш-функция (Justin Sobel).
 * @param key Входная строка.
 * @param mod Размер таблицы.
 * @return Хэш-код в диапазоне [0, mod-1].
 */
size_t jsHash(const string& key, size_t mod) {
    size_t hash = 1315423911;
    for (char c : key) {
        hash ^= ((hash << 5) + static_cast<size_t>(c) + (hash >> 2));
    }
    return hash % mod;
}

/**
 * @brief DJB хэш-функция (Daniel J. Bernstein).
 * @param key Входная строка.
 * @param mod Размер таблицы.
 * @return Хэш-код в диапазоне [0, mod-1].
 */
size_t djbHash(const string& key, size_t mod) {
    size_t hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + static_cast<size_t>(c);
    }
    return hash % mod;
}

struct HashFunctionWrapper {
    string name;                                    ///< Название функции
    string description;                             ///< Краткое описание
    function<size_t(const string&, size_t)> func;   ///< Указатель на функцию
};

vector<HashFunctionWrapper> getHashFunctions() {
    return {
        {"POLY",   "Полиномиальная h = (h*131 + c) % mod", polyHash},
        {"RS",     "RS (Седжвик): два простых числа a,b", rsHash},
        {"JS",     "JS (Sobel): сдвиги и XOR", jsHash},
        {"DJB",    "DJB (Bernstein): h = h*33 + c", djbHash}
    };
}

template<typename HashFunc>
class HashTableWithHash {
public:
    HashTableWithHash(size_t table_size, HashFunc hash_func)
        : tableSize(table_size), buckets(table_size), collisions(0), hashFunc(hash_func) {}

    void insert(const Flight& f) {
        size_t idx = hashFunc(f.getKey(), tableSize);
        auto& bucket = buckets[idx];
        auto it = bucket.find(f.getKey());
        if (it == bucket.end()) {
            bucket.emplace(f.getKey(), vector<Flight>{f});
            if (bucket.size() > 1) collisions++;
        } else {
            it->second.push_back(f);
        }
    }

    vector<Flight> find(const string& key) const {
        size_t idx = hashFunc(key, tableSize);
        const auto& bucket = buckets[idx];
        auto it = bucket.find(key);
        if (it != bucket.end()) return it->second;
        return {};
    }

    size_t getCollisions() const { return collisions; }

private:
    size_t tableSize;
    vector<unordered_map<string, vector<Flight>>> buckets;
    size_t collisions;
    HashFunc hashFunc;
};

vector<Flight> generateFlights(size_t totalSize, size_t uniqueKeysCount) {
    mt19937 gen(RANDOM_SEED);
    uniform_int_distribution<int> keyDist(1, static_cast<int>(uniqueKeysCount));
    uniform_int_distribution<int> prefixDist(0, 7);
    uniform_int_distribution<int> airlineDist(0, 7);
    uniform_int_distribution<int> dateYearDist(2024, 2025);
    uniform_int_distribution<int> dateMonthDist(1, 12);
    uniform_int_distribution<int> dateDayDist(1, 28);
    uniform_int_distribution<int> hourDist(0, 23);
    uniform_int_distribution<int> minuteDist(0, 59);
    uniform_int_distribution<int> secondDist(0, 59);
    uniform_int_distribution<int> passengersDist(0, 300);

    vector<string> prefixes = {"SU", "S7", "FV", "DP", "U6", "UT", "N4", "WZ"};
    vector<string> airlines = {"Aeroflot", "S7 Airlines", "Rossiya", "Pobeda",
                               "Ural Airlines", "Utair", "Nordwind", "Red Wings"};

    vector<Flight> flights;
    flights.reserve(totalSize);

    for (size_t i = 0; i < totalSize; ++i) {
        int keyNum = keyDist(gen);
        string prefix = prefixes[prefixDist(gen)];
        string flightNumber = prefix + to_string(keyNum);
        string airline = airlines[airlineDist(gen)];
        string date = to_string(dateYearDist(gen)) + "-" +
                      to_string(dateMonthDist(gen)) + "-" +
                      to_string(dateDayDist(gen));
        string time = to_string(hourDist(gen)) + ":" +
                      to_string(minuteDist(gen)) + ":" +
                      to_string(secondDist(gen));
        int passengers = passengersDist(gen);
        flights.emplace_back(flightNumber, airline, date, time, passengers);
    }
    return flights;
}

vector<string> getTestKeys(size_t uniqueKeysCount) {
    vector<string> keys;
    vector<string> prefixes = {"SU", "S7", "FV"};
    for (int i = 1; i <= NUM_SEARCHES && i <= (int)uniqueKeysCount; ++i) {
        keys.push_back(prefixes[i % prefixes.size()] + to_string(i));
    }
    return keys;
}

int main() {
    cout << "Сравнение хэш-функций" << endl;
    cout << "Размер массива: " << TOTAL_SIZE << " элементов" << endl;
    cout << "Уникальных ключей: " << UNIQUE_KEYS_COUNT << endl;
    cout << "Размер хэш-таблицы: " << HASH_TABLE_SIZE << endl;
    cout << "Количество поисков: " << NUM_SEARCHES << endl;
    cout << endl;

    cout << "Генерация данных... " << flush;
    vector<Flight> data = generateFlights(TOTAL_SIZE, UNIQUE_KEYS_COUNT);
    vector<string> testKeys = getTestKeys(UNIQUE_KEYS_COUNT);
    cout << "Размер данных: " << data.size() << endl;

    vector<HashFunctionWrapper> hashFunctions = getHashFunctions();

    ofstream csv("hash_functions_results.csv");
    if (!csv.is_open()) {
        cerr << "Ошибка: не удалось создать CSV-файл" << endl;
        return 1;
    }
    csv << "hash_function,collisions,search_time_us,description\n";

    cout << "\nРезультаты тестирования хэш-функций:\n" << endl;
    cout << left << setw(12) << "Функция"
         << setw(15) << "Коллизии"
         << setw(20) << "Время (мкс)"
         << "Описание" << endl;
    cout << string(70, '-') << endl;

    for (const auto& hf : hashFunctions) {
        cout << "Тестирование: " << hf.name << " ... " << flush;

        HashTableWithHash<decltype(hf.func)> ht(HASH_TABLE_SIZE, hf.func);
        for (const auto& f : data) {
            ht.insert(f);
        }

        size_t collisions = ht.getCollisions();

        auto start = high_resolution_clock::now();
        for (const string& key : testKeys) {
            volatile auto result = ht.find(key);
            (void)result;
        }
        auto end = high_resolution_clock::now();
        double searchTime = duration_cast<microseconds>(end - start).count() / (double)testKeys.size();

        csv << hf.name << "," << collisions << "," << searchTime << ",\"" << hf.description << "\"\n";

        cout << " коллизий: " << collisions
             << ", время: " << searchTime << " мкс" << endl;
    }

    csv.close();
    cout << "\nРезультаты сохранены в hash_functions_results.csv" << endl;

    return 0;
}