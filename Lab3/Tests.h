#pragma once

#include "Prng.h"

#include <string>

struct TestResult
{
    std::string name;
    double value;
    bool passed;
};

int* generateBits(IGenerator& generator, int bitCount);
TestResult* runFiveTests(IGenerator& generator, int bitCount, int& testCount);