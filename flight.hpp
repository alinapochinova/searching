/**
 * @file Flight.hpp
 * @brief Заголовочный файл класса Flight.
 * 
 * @details Класс описывает авиарейс и используется 
 *          для тестирования алгоритмов поиска. Ключом поиска является номер рейса.
 */

#pragma once
#include <string>
#include <iostream>

using namespace std;

/**
 * @class Flight
 * @brief Класс, представляющий информацию об авиарейсе.
 * 
 * @details Содержит поля:
 *          - номер рейса (используется как ключ для поиска)
 *          - авиакомпания
 *          - дата прилета
 *          - время прилета
 *          - число пассажиров
 * 
 * @note Операторы сравнения реализованы по правилам:
 *       сначала дата, затем время, затем авиакомпания,
 *       затем число пассажиров по убыванию.
 * @see BSTree, RBTree, HashTable
 */
class Flight {
private:
    string flightNumber;   ///< Номер рейса (уникальный идентификатор для поиска)
    string airline;        ///< Название авиакомпании
    string arrivalDate;    ///< Дата прилета в формате YYYY-MM-DD
    string arrivalTime;    ///< Время прилета в формате HH:MM:SS
    int passengers;        ///< Количество пассажиров (от 0 до 300)

public:
    /**
     * @brief Конструктор по умолчанию.
     * @details Инициализирует все поля значениями по умолчанию:
     *          пустые строки и passengers = 0.
     */
    Flight();

    /**
     * @brief Конструктор с параметрами.
     * @param fn Номер рейса.
     * @param al Авиакомпания.
     * @param date Дата прилета.
     * @param time Время прилета.
     * @param pass Число пассажиров.
     */
    Flight(const string& fn, const string& al,
           const string& date, const string& time, int pass);

    /** @return Номер рейса (ключ для поиска). */
    string getKey() const;

    /** @return Номер рейса. */
    string getFlightNumber() const;

    /** @return Авиакомпания. */
    string getAirline() const;

    /** @return Дата прилета. */
    string getArrivalDate() const;

    /** @return Время прилета. */
    string getArrivalTime() const;

    /** @return Число пассажиров. */
    int getPassengers() const;

    /**
     * @brief Оператор "меньше" для сравнения двух рейсов.
     * @param other Объект для сравнения.
     * @return true, если текущий рейс должен идти раньше other.
     * 
     * @details Правила сравнения (по приоритету):
     * 1. Сравнение по дате прилета.
     * 2. При равенстве дат - по времени прилета.
     * 3. При равенстве времени - по названию авиакомпании.
     * 4. При равенстве авиакомпании - по числу пассажиров (по убыванию).
     */
    bool operator<(const Flight& other) const;

    /**
     * @brief Оператор "больше".
     * @param other Объект для сравнения.
     * @return true, если текущий рейс больше other.
     * @note Реализован через оператор <.
     */
    bool operator>(const Flight& other) const;

    /**
     * @brief Оператор равенства.
     * @param other Объект для сравнения.
     * @return true, если все поля совпадают.
     */
    bool operator==(const Flight& other) const;

    /**
     * @brief Чтение объекта из потока (формат CSV).
     * @param is Входной поток.
     * @param f Объект для заполнения.
     * @return Ссылка на поток.
     * 
     * @details Ожидает строку формата:
     *          "Номер рейса,Авиакомпания,Дата прилета,Время прилета,Число пассажиров"
     *          Например: "SU1234,Aeroflot,2025-05-27,14:30:00,150"
     * 
     * @warning Заголовок файла должен быть пропущен отдельно.
     */
    friend istream& operator>>(istream& is, Flight& f);

    /**
     * @brief Запись объекта в поток (формат CSV без заголовка).
     * @param os Выходной поток.
     * @param f Объект.
     * @return Ссылка на поток.
     */
    friend ostream& operator<<(ostream& os, const Flight& f);
};