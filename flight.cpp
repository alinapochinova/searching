/**
 * @file Flight.cpp
 * @brief Реализация методов класса Flight.
 */

#include "Flight.hpp"
#include <sstream>

using namespace std;

Flight::Flight() 
    : flightNumber(""), airline(""), arrivalDate(""), arrivalTime(""), passengers(0) {}

Flight::Flight(const string& fn, const string& al,
               const string& date, const string& time, int pass)
    : flightNumber(fn), airline(al), arrivalDate(date), arrivalTime(time), passengers(pass) {}

string Flight::getKey() const { return flightNumber; }
string Flight::getFlightNumber() const { return flightNumber; }
string Flight::getAirline() const { return airline; }
string Flight::getArrivalDate() const { return arrivalDate; }
string Flight::getArrivalTime() const { return arrivalTime; }
int Flight::getPassengers() const { return passengers; }

bool Flight::operator<(const Flight& other) const {
    if (arrivalDate != other.arrivalDate)
        return arrivalDate < other.arrivalDate;
    if (arrivalTime != other.arrivalTime)
        return arrivalTime < other.arrivalTime;
    if (airline != other.airline)
        return airline < other.airline;
    return passengers > other.passengers; // убывание
}

bool Flight::operator>(const Flight& other) const {
    return other < *this;
}

bool Flight::operator==(const Flight& other) const {
    return flightNumber == other.flightNumber &&
           airline == other.airline &&
           arrivalDate == other.arrivalDate &&
           arrivalTime == other.arrivalTime &&
           passengers == other.passengers;
}

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
    os << f.flightNumber << ","
       << f.airline << ","
       << f.arrivalDate << ","
       << f.arrivalTime << ","
       << f.passengers;
    return os;
}