#include <string>

#include "format.h"

using std::string;


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    long hh = (seconds / 3600);
    long mm = (seconds % 3600) / 60;
    long ss = (seconds % 3600) % 60;
    hh = hh % 24;
    string A = ((hh<10)? ("0" + std::to_string(hh)): (std::to_string(hh)));
    string B = ((mm<10)? ("0" + std::to_string(mm)): (std::to_string(mm)));
    string C = ((ss<10)? ("0" + std::to_string(ss)): (std::to_string(ss)));
    return A + ":" + B + ":" + C;
}