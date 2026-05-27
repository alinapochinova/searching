/**
 * @file generate_data.cpp
 * @brief Генератор тестовых данных для лабораторной работы №2 (поиск данных).
 * 
 * @author Ваше имя
 * @student_id Номер студенческого
 * @group Номер группы
 * @date 2026
 * @version 1.0
 * 
 * @details Создаёт CSV-файлы с информацией об авиарейсах.
 *          Номера рейсов (ключи поиска) генерируются из ограниченного набора,
 *          что обеспечивает многократные повторения для тестирования поиска
 *          всех вхождений.
 * 
 * @section usage Использование
 * @code
 * ./generate_data
 * @endcode
 * 
 * @section output Выходные данные
 * Файлы сохраняются в папку data/ с именами flights_{size}.csv,
 * где size – количество записей.
 * 
 * @see Flight (класс из основной программы)
 * @see main_benchmark.cpp
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <sstream>
#include <sys/stat.h> 

/**
 * @brief Количество уникальных номеров рейсов.
 * @details Чем меньше это число, тем больше дубликатов в массиве.
 *          При размере N каждый ключ в среднем встретится N / UNIQUE_COUNT раз.
 */
const int UNIQUE_FLIGHT_NUMBERS = 2000;

/** @brief Фиксированный seed для генератора (воспроизводимость). */
const unsigned int RANDOM_SEED = 42;

/** @brief Список авиакомпаний для случайного выбора. */
const std::vector<std::string> AIRLINES = {
    "Aeroflot", "S7 Airlines", "Rossiya", "Pobeda",
    "Ural Airlines", "Utair", "Nordwind", "Red Wings"
};

/** @brief Префиксы для номеров рейсов. */
const std::vector<std::string> PREFIXES = {"SU", "S7", "FV", "DP", "U6", "UT", "N4", "WZ"};

/**
 * @brief Глобальный генератор случайных чисел (Mersenne Twister).
 * @details Инициализируется фиксированным seed для воспроизводимости.
 */
std::mt19937 gen(RANDOM_SEED);

/**
 * @brief Генерация случайной даты прилёта.
 * @return Строка в формате YYYY-MM-DD.
 * @details Диапазон: 2024-01-01 до 2025-12-28.
 */
std::string randomDate() {
    std::uniform_int_distribution<int> yearDist(2024, 2025);
    std::uniform_int_distribution<int> monthDist(1, 12);
    std::uniform_int_distribution<int> dayDist(1, 28);
    
    std::stringstream ss;
    ss << yearDist(gen) << "-"
       << std::setw(2) << std::setfill('0') << monthDist(gen) << "-"
       << std::setw(2) << std::setfill('0') << dayDist(gen);
    return ss.str();
}

/**
 * @brief Генерация случайного времени прилёта.
 * @return Строка в формате HH:MM:SS.
 * @details Диапазон: 00:00:00 до 23:59:59.
 */
std::string randomTime() {
    std::uniform_int_distribution<int> hourDist(0, 23);
    std::uniform_int_distribution<int> minuteDist(0, 59);
    std::uniform_int_distribution<int> secondDist(0, 59);
    
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hourDist(gen) << ":"
       << std::setw(2) << std::setfill('0') << minuteDist(gen) << ":"
       << std::setw(2) << std::setfill('0') << secondDist(gen);
    return ss.str();
}

/**
 * @brief Генерация случайного названия авиакомпании.
 * @return Одна из строк в списке AIRLINES.
 */
std::string randomAirline() {
    std::uniform_int_distribution<size_t> dist(0, AIRLINES.size() - 1);
    return AIRLINES[dist(gen)];
}

/**
 * @brief Генерация случайного числа пассажиров.
 * @return Число от 0 до 300.
 */
int randomPassengers() {
    std::uniform_int_distribution<int> dist(0, 300);
    return dist(gen);
}

/**
 * @brief Генерация номера рейса из ограниченного множества.
 * @return Строка вида "SU1234", где число от 1 до UNIQUE_FLIGHT_NUMBERS.
 * @details Номер рейса формируется как случайный префикс + число.
 *          Такая схема гарантирует, что количество различных ключей
 *          не превышает UNIQUE_FLIGHT_NUMBERS * PREFIXES.size(),
 *          что создаёт множество дубликатов в больших массивах.
 */
std::string randomFlightNumber() {
    std::uniform_int_distribution<int> numDist(1, UNIQUE_FLIGHT_NUMBERS);
    std::uniform_int_distribution<size_t> prefixDist(0, PREFIXES.size() - 1);
    std::string prefix = PREFIXES[prefixDist(gen)];
    int number = numDist(gen);
    return prefix + std::to_string(number);
}

/**
 * @brief Вспомогательная структура для временного хранения записи.
 * @details Не является частью основной программы, используется только
 *          при генерации CSV-файлов.
 */
struct FlightRecord {
    std::string flightNumber;   ///< Номер рейса (ключ)
    std::string airline;        ///< Авиакомпания
    std::string arrivalDate;    ///< Дата прилёта
    std::string arrivalTime;    ///< Время прилёта
    int passengers;             ///< Число пассажиров
};

/**
 * @brief Сохранение вектора записей в CSV-файл.
 * @param flights Вектор структур FlightRecord.
 * @param filename Имя выходного файла.
 * @details Заголовок файла соответствует ожиданиям класса Flight
 *          из основной программы.
 */
void saveToCSV(const std::vector<FlightRecord>& flights, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
        return;
    }
    // Заголовок должен совпадать с форматом operator>> в классе Flight
    file << "Номер рейса,Авиакомпания,Дата прилета,Время прилета,Число пассажиров\n";
    for (const auto& f : flights) {
        file << f.flightNumber << ","
             << f.airline << ","
             << f.arrivalDate << ","
             << f.arrivalTime << ","
             << f.passengers << "\n";
    }
    file.close();
}

/**
 * @brief Главная функция генератора данных.
 * @return 0 при успешном завершении.
 * @details Последовательно генерирует файлы для следующих размеров:
 *          100, 500, 1000, 5000, 10000, 20000, 30000, 50000, 75000, 100000,
 *          200000, 300000, 500000, 750000, 1000000.
 * @post В папке data/ создаются файлы flights_{size}.csv.
 */
int main() {
    // Список размеров массивов для генерации
    std::vector<size_t> sizes = {
        100, 500, 1000, 5000, 10000, 20000, 30000, 50000,
        75000, 100000, 200000, 300000, 500000, 750000, 1000000
    };

    // Создаём папку data/, если её нет
    mkdir("data", 0755);

    for (size_t n : sizes) {
        std::cout << "Генерация " << n << " записей..." << std::endl;
        std::vector<FlightRecord> flights;
        flights.reserve(n);
        
        for (size_t i = 0; i < n; ++i) {
            FlightRecord rec;
            rec.flightNumber = randomFlightNumber();
            rec.airline = randomAirline();
            rec.arrivalDate = randomDate();
            rec.arrivalTime = randomTime();
            rec.passengers = randomPassengers();
            flights.push_back(rec);
        }
        
        std::string filename = "data/flights_" + std::to_string(n) + ".csv";
        saveToCSV(flights, filename);
    }
    
    std::cout << "Генерация завершена. Файлы сохранены в папку data/" << std::endl;
    return 0;
}