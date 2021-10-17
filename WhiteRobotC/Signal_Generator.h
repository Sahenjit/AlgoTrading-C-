//
// Created by Shankar on 25/09/2021.
//

#ifndef WHITEROBOTC_SIGNAL_GENERATOR_H
#define WHITEROBOTC_SIGNAL_GENERATOR_H

#include <vector>
#include <algorithm>
#include <numeric>

class Signal_Generator
{
public:
    Signal_Generator();
    double movingAverage(std::vector<double> prices, int windowSize);
    double movingSlope(const std::vector<double> prices, int windowSize);
};


#endif //WHITEROBOTC_SIGNAL_GENERATOR_H
