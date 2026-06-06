/**
 * @file hash_heatmap.cpp
 * @brief Анализ распределения элементов по корзинам хэш-таблицы.
 * 
 * @details Строит хэш-таблицу на 500000 элементах (2000 уникальных ключей),
 *          анализирует распределение элементов по корзинам, выводит статистику.
 *          Для визуализации используется Python-скрипт plot_heatmap.py.
 * 
 * @section output Выходные данные
 * - hash_distribution.txt: текстовый отчёт со статистикой
 * - bucket_load.csv: данные о загрузке корзин
 * 
 * @section plot Для построения тепловой карты:
 * @code
 * python3 plot_heatmap.py
 * @endcode
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <map>
#include "Flight.hpp"
#include "HashTable.hpp"

using namespace std;

const size_t TOTAL_SIZE = 500000;
const size_t UNIQUE_KEYS_COUNT = 2000;
const size_t HASH_TABLE_SIZE = 200003;
const unsigned int RANDOM_SEED = 42;

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

struct DistributionStats {
    size_t totalBuckets;
    size_t nonEmptyBuckets;
    size_t maxLoad;
    double avgLoad;
    double loadFactor;
    size_t collisions;
    map<size_t, size_t> loadDistribution;
};

DistributionStats analyzeDistribution(const HashTable& ht) {
    DistributionStats stats;
    stats.totalBuckets = HASH_TABLE_SIZE;
    stats.collisions = ht.getCollisions();
    stats.nonEmptyBuckets = 0;
    stats.maxLoad = 0;
    stats.loadDistribution.clear();
    
    vector<size_t> bucketSizes = ht.getBucketSizes();
    
    for (size_t load : bucketSizes) {
        if (load > 0) {
            stats.nonEmptyBuckets++;
            stats.maxLoad = max(stats.maxLoad, load);
        }
        stats.loadDistribution[load]++;
    }
    
    size_t totalLoad = 0;
    for (const auto& [load, count] : stats.loadDistribution) {
        totalLoad += load * count;
    }
    if (stats.nonEmptyBuckets > 0) {
        stats.avgLoad = static_cast<double>(totalLoad) / stats.nonEmptyBuckets;
    }
    
    stats.loadFactor = static_cast<double>(UNIQUE_KEYS_COUNT) / HASH_TABLE_SIZE;
    
    return stats;
}

void saveTextReport(const DistributionStats& stats, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot create file " << filename << endl;
        return;
    }
    
    file << "HASH TABLE HEATMAP REPORT\n";
    file << "\n";
    file << "Experiment parameters:\n";
    file << "  Array size: " << TOTAL_SIZE << "\n";
    file << "  Unique keys: " << UNIQUE_KEYS_COUNT << "\n";
    file << "  Table size: " << stats.totalBuckets << "\n";
    file << "  Load factor: " << fixed << setprecision(4) << stats.loadFactor << "\n";
    file << "\n";
    file << "Distribution statistics:\n";
    file << "  Total buckets: " << stats.totalBuckets << "\n";
    
    // Исправление: используем find вместо operator[]
    size_t emptyBuckets = 0;
    auto it = stats.loadDistribution.find(0);
    if (it != stats.loadDistribution.end()) {
        emptyBuckets = it->second;
    }
    file << "  Empty buckets: " << emptyBuckets << "\n";
    
    file << "  Non-empty buckets: " << stats.nonEmptyBuckets << "\n";
    file << "  Max load per bucket: " << stats.maxLoad << "\n";
    file << "  Average load (non-empty): " << fixed << setprecision(2) << stats.avgLoad << "\n";
    file << "  Collisions: " << stats.collisions << "\n";
    file << "\n";
    file << "Load distribution histogram:\n";
    file << "\n";
    
    for (const auto& [load, count] : stats.loadDistribution) {
        if (load == 0) continue;
        file << "  " << setw(4) << load << " element(s): " << setw(8) << count << " buckets\n";
    }
    file << "\n  Empty buckets: " << emptyBuckets << "\n";
    file << "\n";
    file << "Quality assessment:\n";
    
    double uniformity = (double)stats.nonEmptyBuckets / stats.totalBuckets * 100;
    file << "  Table utilization: " << fixed << setprecision(2) << uniformity << "%\n";
    
    if (stats.maxLoad <= 2) {
        file << "  Grade: EXCELLENT\n";
    } else if (stats.maxLoad <= 5) {
        file << "  Grade: GOOD\n";
    } else if (stats.maxLoad <= 10) {
        file << "  Grade: SATISFACTORY\n";
    } else {
        file << "  Grade: POOR\n";
    }
    
    file.close();
    cout << "Report saved to " << filename << endl;
}

void saveBucketLoadCSV(const HashTable& ht, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot create file " << filename << endl;
        return;
    }
    
    vector<size_t> bucketSizes = ht.getBucketSizes();
    
    file << "bucket_index,load\n";
    for (size_t i = 0; i < bucketSizes.size(); ++i) {
        file << i << "," << bucketSizes[i] << "\n";
    }
    
    file.close();
    cout << "Bucket load data saved to " << filename << endl;
}

int main() {
    cout << "Generating data..." << endl;
    vector<Flight> data = generateFlights(TOTAL_SIZE, UNIQUE_KEYS_COUNT);
    
    cout << "Building hash table..." << endl;
    HashTable ht(HASH_TABLE_SIZE);
    for (const auto& f : data) {
        ht.insert(f);
    }
    cout << "Collisions: " << ht.getCollisions() << endl;
    
    cout << "Analyzing distribution..." << endl;
    DistributionStats stats = analyzeDistribution(ht);
    
    saveTextReport(stats, "hash_distribution.txt");
    saveBucketLoadCSV(ht, "bucket_load.csv");
    
    cout << "\nTo generate heatmap and histogram, run: python3 plot_heatmap.py" << endl;
    
    return 0;
}