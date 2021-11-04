//
// Created by Shankar on 04/11/2021.
//

#ifndef WHITEROBOTC_DATE_H
#define WHITEROBOTC_DATE_H
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
using namespace std;
class Date {
    std::string day = "";
    std::string month = "";
    std::string year = "";
    std::vector<std::string> seglist;
public:
    Date(std::string userinput);
    std::string reformat_date();
};


#endif //WHITEROBOTC_DATE_H
