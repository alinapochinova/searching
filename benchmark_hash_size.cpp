/**
 * @file benchmark_hash_size.cpp
 * @brief Бенчмарк зависимости коллизий и времени поиска от размера хэш-таблицы.
 * 
 * @details Фиксируется массив из 500 000 элементов (2000 уникальных ключей).
 *          Размер хэш-таблицы варьируется от 101 до 500009 (простые числа).
 *          Для каждого размера измеряется количество коллизий и время поиска.
 * 
 * @author Починова Алина
 * @date 2026
 * 
 * @section usage Запуск
 * @code
 * ./benchmark_hash_size
 * @endcode
 * 
 * @section output Выходные данные
 * Файл hash_size_results.csv с колонками:
 * - table_size: размер хэш-таблицы
 * - collisions: количество коллизий
 * - search_time_us: время поиска (микросекунды)
 * - load_factor: коэффициент заполнения (уникальные_ключи / table_size)
 */

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <random>
#include <set>
#include <cmath>
#include "Flight.hpp"
#include "HashTable.hpp"

using namespace std;
using namespace chrono;

const size_t TOTAL_SIZE = 500000;          ///< Фиксированный размер массива
const size_t UNIQUE_KEYS_COUNT = 2000;     ///< Количество уникальных ключей
const int NUM_SEARCHES = 100;              ///< Количество поисков для замера
const unsigned int RANDOM_SEED = 42;       ///< Фиксированный seed для воспроизводимости

/**
 * @brief Проверка, является ли число простым.
 * @param n Проверяемое число.
 * @return true, если число простое.
 */
bool isPrime(size_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

/**
 * @brief Генерирует список простых чисел для размеров хэш-таблицы.
 * @return Вектор простых чисел в диапазоне от 100 до 600000.
 */
vector<size_t> generateTableSizes() {
    vector<size_t> sizes = {
        101, 251, 503, 1009, 2003, 4001, 8009, 16001, 32003, 64007,
        100003, 150001, 200003, 250007, 300017, 350003, 400009, 450001, 500009
    };
    // Фильтруем только простые числа 
    vector<size_t> result;
    for (size_t s : sizes) {
        if (isPrime(s)) result.push_back(s);
    }
    return result;
}

/**
 * @brief Генерирует массив объектов Flight с фиксированным числом уникальных ключей.
 * @param totalSize Общее количество элементов.
 * @param uniqueKeysCount Количество различных номеров рейсов.
 * @return Вектор объектов Flight.
 */
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

/**
 * @brief Выбирает фиксированные ключи для поиска (первые NUM_SEARCHES номеров).
 * @param uniqueKeysCount Общее количество уникальных ключей.
 * @return Вектор ключей.
 */
vector<string> getTestKeys(size_t uniqueKeysCount) {
    vector<string> keys;
    vector<string> prefixes = {"SU", "S7", "FV"};
    for (int i = 1; i <= NUM_SEARCHES && i <= (int)uniqueKeysCount; ++i) {
        keys.push_back(prefixes[i % prefixes.size()] + to_string(i));
    }
    return keys;
}

/**
 * @brief Замеряет время поиска в хэш-таблице.
 * @param ht Хэш-таблица.
 * @param keys Вектор ключей для поиска.
 * @return Среднее время одного поиска в микросекундах.
 */
double measureSearchTime(const HashTable& ht, const vector<string>& keys) {
    auto start = high_resolution_clock::now();
    for (const string& key : keys) {
        volatile auto result = ht.find(key);
        (void)result;
    }
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / (double)keys.size();
}

/**
 * @brief Главная функция бенчмарка.
 * @return 0 при успешном выполнении.
 * 
 * @details Последовательно тестирует хэш-таблицы разных размеров:
 *          - Генерирует фиксированный массив данных (один раз).
 *          - Для каждого размера таблицы строит новую хэш-таблицу,
 *            вставляет все элементы, замеряет коллизии и время поиска.
 *          - Сохраняет результаты в CSV.
 * 
 * @note Ожидаемый результат:
 *       - При малом размере таблицы (101) – много коллизий, время поиска большое.
 *       - При оптимальном размере (~2 * UNIQUE_KEYS_COUNT = 4000) – коллизий мало.
 *       - При дальнейшем увеличении размера – коллизии стремятся к 0,
 *         время поиска стабилизируется, но растёт расход памяти.
 */
int main() {
    cout << "Бенчмарк: влияние размера хэш-таблицы" << endl;
    cout << "Размер массива: " << TOTAL_SIZE << " элементов" << endl;
    cout << "Уникальных ключей: " << UNIQUE_KEYS_COUNT << endl;
    cout << "Количество поисков для замера: " << NUM_SEARCHES << endl;
    cout << endl;

    // 1. Генерация данных (один раз для всех экспериментов)
    cout << "Генерация данных... " << flush;
    vector<Flight> data = generateFlights(TOTAL_SIZE, UNIQUE_KEYS_COUNT);
    vector<string> testKeys = getTestKeys(UNIQUE_KEYS_COUNT);
    cout << "Размер данных: " << data.size() << endl;

    // 2. Получение списка размеров таблицы
    vector<size_t> tableSizes = generateTableSizes();

    // 3. Открываем CSV для записи результатов
    ofstream csv("hash_size_results.csv");
    if (!csv.is_open()) {
        cerr << "Ошибка: не удалось создать CSV-файл" << endl;
        return 1;
    }
    csv << "table_size,collisions,search_time_us,load_factor\n";

    // 4. Основной цикл по размерам таблицы
    for (size_t tableSize : tableSizes) {
        cout << "Тестирование: размер таблицы = " << tableSize << " ... " << flush;

        // Построение хэш-таблицы
        HashTable ht(tableSize);
        for (const auto& f : data) {
            ht.insert(f);
        }

        // Получение коллизий
        size_t collisions = ht.getCollisions();

        // Замер времени поиска
        double searchTime = measureSearchTime(ht, testKeys);

        // Коэффициент заполнения (уникальные ключи / размер таблицы)
        double loadFactor = (double)UNIQUE_KEYS_COUNT / tableSize;

        // Запись в CSV
        csv << tableSize << "," << collisions << "," << searchTime << "," << loadFactor << "\n";

        cout << " коллизий: " << collisions 
             << ", время: " << searchTime << " мкс"
             << ", загрузка: " << loadFactor << endl;
    }

    csv.close();
    cout << "\nРезультаты сохранены в hash_size_results.csv" << endl;

    return 0;
}