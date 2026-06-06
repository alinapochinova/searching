/**
 * @file Flight.cpp
 * @brief Реализация ввода/вывода Flight.
 */

#include "Flight.hpp"
#include <sstream>

using namespace std;

// Конструктор с параметрами
Flight::Flight(const string& fn, const string& al,
               const string& date, const string& time, int pass)
    : flightNumber(fn), airline(al), arrivalDate(date), arrivalTime(time), passengers(pass) {}

istream& operator>>(istream& is, Flight& f) {
    string line;
    if (!getline(is, line)) return is;
    stringstream ss(line);
    getline(ss, f.flightNumber, ',');
    getline(ss, f.airline, ',');
    getline(ss, f.arrivalDate, ',');
    getline(ss, f.arrivalTime, ',');
    string passStr;
    getline(ss, passStr, ',');
    f.passengers = stoi(passStr);
    return is;
}

ostream& operator<<(ostream& os, const Flight& f) {
    os << f.flightNumber << ','
       << f.airline << ','
       << f.arrivalDate << ','
       << f.arrivalTime << ','
       << f.passengers;
    return os;
}