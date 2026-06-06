#pragma once

struct SampleStats
{
    double mean;
    double deviation;
    double variation;
};

struct ChiSquareResult
{
    double value;
    double critical;
    bool passed;
};

SampleStats calculateStats(const int* values, int size);
ChiSquareResult chiSquareUniform(const int* values, int size, int minValue, int maxValue, int intervals);
ChiSquareResult chiSquareRandomness(const int* values, int size);