/**
 * @file benchmark_unique_keys.cpp
 * @brief Бенчмарк зависимости времени поиска от количества уникальных ключей.
 * 
 * @details Фиксируется размер массива (1 000 000 элементов), но меняется
 *          количество различных номеров рейсов (от 100 до 500 000).
 *          Для каждого значения измеряется время поиска в BST, RBT и HashTable.
 *          Ожидаемое поведение: время поиска в BST/RBT должно расти как O(log U),
 *          в хэш-таблице – как O(1) с медленным ростом из-за коллизий.
 *          Если наблюдаются немонотонные колебания или падение времени,
 *          это может быть вызвано:
 *          1. Эффектами кэширования процессора (данные малых U помещаются в кэш).
 *          2. Шумом от фоновых процессов ОС.
 *          3. Недостаточным количеством поисков (NUM_SEARCHES) для усреднения.
 *          4. Случайным выбором ключей (разные ключи могут иметь разную глубину).
 * @warning 
 * 
 * @recommendation Для получения гладкого графика:
 *          - Увеличить NUM_SEARCHES до 100.
 *          - Запустить бенчмарк несколько раз и усреднить результаты.
 *          - Использовать одни и те же ключи для всех U (например, первые 10 уникальных).
 *          - Отключить турбобуст процессора и закрыть фоновые приложения.
 * @author Починова Алина
 * @date 2026
 * 
 * @section usage Запуск
 * @code
 * ./benchmark_unique_keys
 * @endcode
 * 
 * @section output Выходные данные
 * Файл unique_keys_results.csv с колонками:
 * - unique_keys: количество уникальных ключей
 * - bst_time_us: время поиска в BST (микросекунды)
 * - rbt_time_us: время поиска в RBT (микросекунды)
 * - hash_time_us: время поиска в хэш-таблице (микросекунды)
 */

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <random>
#include <set>
#include <algorithm>
#include "Flight.hpp"
#include "BSTree.hpp"
#include "RBTree.hpp"
#include "HashTable.hpp"

using namespace std;
using namespace chrono;

const size_t TOTAL_SIZE = 1000000;           ///< Фиксированный размер массива
const size_t HASH_TABLE_SIZE = 200003;       ///< Фиксированный размер хэш-таблицы
const int NUM_SEARCHES = 10;                 ///< Количество поисков для замера
const unsigned int RANDOM_SEED = 42;         ///< Фиксированный seed для воспроизводимости

/**
 * @brief Генерирует массив объектов Flight с заданным количеством уникальных номеров рейсов.
 * @param totalSize Общее количество элементов.
 * @param uniqueKeysCount Количество различных номеров рейсов.
 * @return Вектор объектов Flight.
 * 
 * @details Номера рейсов генерируются в диапазоне [1, uniqueKeysCount].
 *          Используется фиксированный seed для воспроизводимости.
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
        // Генерация номера рейса (ключ)
        int keyNum = keyDist(gen);
        string prefix = prefixes[prefixDist(gen)];
        string flightNumber = prefix + to_string(keyNum);

        // Генерация авиакомпании
        string airline = airlines[airlineDist(gen)];

        // Генерация даты
        string date = to_string(dateYearDist(gen)) + "-" +
                      to_string(dateMonthDist(gen)) + "-" +
                      to_string(dateDayDist(gen));

        // Генерация времени
        string time = to_string(hourDist(gen)) + ":" +
                      to_string(minuteDist(gen)) + ":" +
                      to_string(secondDist(gen));

        // Генерация числа пассажиров
        int passengers = passengersDist(gen);

        flights.emplace_back(flightNumber, airline, date, time, passengers);
    }

    return flights;
}

/**
 * @brief Выбирает NUM_SEARCHES случайных ключей из массива для последующего поиска.
 * @param flights Массив объектов.
 * @return Вектор уникальных ключей.
 */
vector<string> selectTestKeys(const vector<Flight>& flights) {
    set<string> uniqueKeys;
    for (const auto& f : flights) {
        uniqueKeys.insert(f.getKey());
        if (uniqueKeys.size() >= NUM_SEARCHES) break;
    }
    return vector<string>(uniqueKeys.begin(), uniqueKeys.end());
}

/**
 * @brief Замеряет среднее время поиска в структуре данных.
 * @tparam Structure Тип структуры (должен иметь метод find(const string&) const).
 * @param structure Структура для поиска.
 * @param keys Вектор ключей для поиска.
 * @return Среднее время одного поиска в микросекундах.
 */
template<typename T>
double measureSearchTime(const T& structure, const vector<string>& keys) {
    auto start = high_resolution_clock::now();
    for (const string& key : keys) {
        volatile auto result = structure.find(key);
        (void)result; // подавляем предупреждение
    }
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / (double)keys.size();
}

/**
 * @brief Главная функция бенчмарка.
 * @return 0 при успешном выполнении.
 */
int main() {
    // Список значений для количества уникальных ключей
    vector<size_t> uniqueKeysList = {
        100, 500, 1000, 5000, 10000, 50000, 100000, 200000, 300000, 400000, 500000
    };

    // Открываем CSV для записи результатов
    ofstream csv("unique_keys_results.csv");
    if (!csv.is_open()) {
        cerr << "Ошибка: не удалось создать CSV-файл" << endl;
        return 1;
    }
    csv << "unique_keys,bst_time_us,rbt_time_us,hash_time_us\n";

    cout << "Бенчмарк: зависимость времени поиска от числа уникальных ключей" << endl;
    cout << "Фиксированный размер массива: " << TOTAL_SIZE << " элементов" << endl;
    cout << "Размер хэш-таблицы: " << HASH_TABLE_SIZE << endl;
    cout << endl;

    for (size_t uniqueCount : uniqueKeysList) {
        if (uniqueCount > TOTAL_SIZE) {
            cout << "Пропуск: uniqueCount=" << uniqueCount << " > TOTAL_SIZE" << endl;
            continue;
        }

        cout << "Тестирование: уникальных ключей = " << uniqueCount << " ... " << flush;

        // 1. Генерация данных
        vector<Flight> data = generateFlights(TOTAL_SIZE, uniqueCount);

        // 2. Выбор ключей для поиска
        vector<string> testKeys = selectTestKeys(data);

        // 3. Построение BST
        BSTree bst;
        for (const auto& f : data) bst.insert(f);

        // 4. Построение RBT
        RBTree rbt;
        for (const auto& f : data) rbt.insert(f);

        // 5. Построение хэш-таблицы
        HashTable ht(HASH_TABLE_SIZE);
        for (const auto& f : data) ht.insert(f);

        // 6. Замеры времени
        double bstTime = measureSearchTime(bst, testKeys);
        double rbtTime = measureSearchTime(rbt, testKeys);
        double hashTime = measureSearchTime(ht, testKeys);

        // 7. Запись в CSV
        csv << uniqueCount << "," << bstTime << "," << rbtTime << "," << hashTime << "\n";

        cout << " BST: " << bstTime << " мкс, RBT: " << rbtTime << " мкс, Hash: " << hashTime << " мкс" << endl;
    }

    csv.close();
    cout << "\nРезультаты сохранены в unique_keys_results.csv" << endl;

    return 0;
}