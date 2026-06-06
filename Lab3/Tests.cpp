#include "Tests.h"

#include <cmath>

int* generateBits(IGenerator& generator, int bitCount)
{
    //создаем динамический массив битов
    int* bits = new int[bitCount];

    int position = 0;

    while (position < bitCount)
    {
        uint32_t value = generator.next();

        for (int i = 0; i < 32 && position < bitCount; i++)
        {
            bits[position] = (value >> i) & 1u;
            position++;
        }
    }

    return bits;
}

static TestResult frequencyTest(const int* bits, int bitCount)
{
    int ones = 0;

    for (int i = 0; i < bitCount; i++)
    {
        ones += bits[i];
    }

    //доля единиц должна быть близка к 0.5
    double proportion = static_cast<double>(ones) / bitCount;

    bool passed = std::abs(proportion - 0.5) < 0.01;

    return TestResult{ "frequency", proportion, passed };
}

static TestResult blockFrequencyTest(const int* bits, int bitCount)
{
    const int blockSize = 128;

    int blocks = bitCount / blockSize;
    int passedBlocks = 0;

    for (int b = 0; b < blocks; b++)
    {
        int ones = 0;

        for (int i = 0; i < blockSize; i++)
        {
            ones += bits[b * blockSize + i];
        }

        double proportion = static_cast<double>(ones) / blockSize;

        if (std::abs(proportion - 0.5) < 0.09)
        {
            passedBlocks++;
        }
    }

    //доля успешно прошедших блоков
    double ratio = static_cast<double>(passedBlocks) / blocks;

    bool passed = ratio > 0.95;

    return TestResult{ "block_frequency", ratio, passed };
}

static TestResult runsTest(const int* bits, int bitCount)
{
    int runs = 1;

    for (int i = 1; i < bitCount; i++)
    {
        if (bits[i] != bits[i - 1])
        {
            runs++;
        }
    }

    //ожидаемое количество серий
    double expected = bitCount / 2.0;

    double ratio = runs / expected;

    bool passed = ratio > 0.95 && ratio < 1.05;

    return TestResult{ "runs", ratio, passed };
}

static TestResult longestRunTest(const int* bits, int bitCount)
{
    int longest = 0;
    int current = 0;

    for (int i = 0; i < bitCount; i++)
    {
        if (bits[i] == 1)
        {
            current++;
        }
        else
        {
            if (current > longest)
            {
                longest = current;
            }

            current = 0;
        }
    }

    if (current > longest)
    {
        longest = current;
    }

    //примерная ожидаемая длина самой длинной серии
    double expected = std::log2(static_cast<double>(bitCount));

    bool passed = longest > expected * 0.5 && longest < expected * 2.0;

    return TestResult{ "longest_run", static_cast<double>(longest), passed };
}

static TestResult serialTest(const int* bits, int bitCount)
{
    int counts[4] = { 0, 0, 0, 0 };

    for (int i = 1; i < bitCount; i++)
    {
        int index = bits[i - 1] * 2 + bits[i];
        counts[index]++;
    }

    //ожидаемая частота каждой пары битов
    double expected = static_cast<double>(bitCount - 1) / 4.0;

    double chi = 0.0;

    for (int i = 0; i < 4; i++)
    {
        double difference = counts[i] - expected;
        chi += difference * difference / expected;
    }

    //критическое значение для 3 степеней свободы и уровня значимости 0.05
    bool passed = chi < 7.815;

    return TestResult{ "serial_chi_square", chi, passed };
}

TestResult* runFiveTests(IGenerator& generator, int bitCount, int& testCount)
{
    testCount = 5;

    //получаем битовую последовательность
    int* bits = generateBits(generator, bitCount);

    //создаем массив результатов
    TestResult* results = new TestResult[testCount];

    results[0] = frequencyTest(bits, bitCount);
    results[1] = blockFrequencyTest(bits, bitCount);
    results[2] = runsTest(bits, bitCount);
    results[3] = longestRunTest(bits, bitCount);
    results[4] = serialTest(bits, bitCount);

    delete[] bits;

    return results;
}