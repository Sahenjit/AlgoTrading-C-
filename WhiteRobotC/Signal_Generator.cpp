//
// Created by Shankar on 25/09/2021.
//

#include "Signal_Generator.h"


Signal_Generator::Signal_Generator() {}

// Simple moving average function
double Signal_Generator::movingAverage(std::vector<double> prices, int windowSize)
{
    std::vector<double> window;
    window = std::vector < double > (prices.end() - windowSize, prices.end());
    auto n = window.size();
    double average = 0.0;
    if (n != 0) {
        average = accumulate(window.begin(), window.end(), 0.0) / n;
    }
    return average;
}

// Moving slope function
double Signal_Generator::movingSlope(const std::vector<double> prices, int windowSize)
{
    //from https://stackoverflow.com/questions/18939869/how-to-get-the-slope-of-a-linear-regression-line-using-c

    std::vector<double> y;
    y = std::vector < double >(prices.end() - windowSize, prices.end());
    auto n = y.size();

    std::vector<double> x(n);
    iota(x.begin(), x.end(), 0); // x will become: [0..n]

    const auto s_x = std::accumulate(x.begin(), x.end(), 0.0);
    const auto s_y = std::accumulate(y.begin(), y.end(), 0.0);
    const auto s_xx = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
    const auto s_xy = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
    const auto slope = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
    return slope;
}


