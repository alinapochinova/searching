/**
 * @file Flight.hpp
 * @brief Класс авиарейса (ключ - номер рейса).
 */

#pragma once
#include <string>
#include <iostream>

using namespace std;

/**
 * @class Flight
 * @brief Представляет информацию об авиарейсе.
 * Поля: номер рейса (ключ), авиакомпания, дата/время прилёта, число пассажиров.
 */
class Flight {
private:
    string flightNumber;   ///< Номер рейса (ключ поиска)
    string airline;
    string arrivalDate;
    string arrivalTime;
    int passengers;

public:
    Flight() = default;

    /**
     * @brief Конструктор с параметрами.
     * @param fn Номер рейса.
     * @param al Авиакомпания.
     * @param date Дата прилёта.
     * @param time Время прилёта.
     * @param pass Число пассажиров.
     */
    Flight(const string& fn, const string& al,
           const string& date, const string& time, int pass);

    /// Возвращает ключ (номер рейса) по константной ссылке (без копирования).
    const string& getKey() const { return flightNumber; }

    /// Чтение из CSV-потока (формат: номер,авиа,дата,время,пассажиры).
    friend istream& operator>>(istream& is, Flight& f);

    /// Запись в CSV-поток.
    friend ostream& operator<<(ostream& os, const Flight& f);
};