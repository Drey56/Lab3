#include "Statistics.h"

#include <cmath>

SampleStats calculateStats(const int* values, int size)
{
    SampleStats result{ 0.0, 0.0, 0.0 };

    //считаем сумму элементов
    double sum = 0.0;

    for (int i = 0; i < size; i++)
    {
        sum += values[i];
    }

    //считаем среднее значение
    result.mean = sum / size;

    //считаем сумму квадратов отклонений
    double squareSum = 0.0;

    for (int i = 0; i < size; i++)
    {
        double difference = values[i] - result.mean;
        squareSum += difference * difference;
    }

    //считаем стандартное отклонение
    result.deviation = std::sqrt(squareSum / size);

    //считаем коэффициент вариации
    if (result.mean != 0.0)
    {
        result.variation = result.deviation / result.mean;
    }

    return result;
}

ChiSquareResult chiSquareUniform(const int* values, int size, int minValue, int maxValue, int intervals)
{
    //критическое значение для 9 степеней свободы и уровня значимости 0.05
    const double critical = 16.919;

    //создаем массив частот
    int* observed = new int[intervals];

    for (int i = 0; i < intervals; i++)
    {
        observed[i] = 0;
    }

    //считаем ширину одного интервала
    double width = static_cast<double>(maxValue - minValue + 1) / intervals;

    for (int i = 0; i < size; i++)
    {
        int index = static_cast<int>((values[i] - minValue) / width);

        if (index < 0)
        {
            index = 0;
        }

        if (index >= intervals)
        {
            index = intervals - 1;
        }

        observed[index]++;
    }

    //считаем ожидаемую частоту
    double expected = static_cast<double>(size) / intervals;

    double chi = 0.0;

    for (int i = 0; i < intervals; i++)
    {
        double difference = observed[i] - expected;
        chi += difference * difference / expected;
    }

    delete[] observed;

    return ChiSquareResult{ chi, critical, chi < critical };
}

ChiSquareResult chiSquareRandomness(const int* values, int size)
{
    //критическое значение для 1 степени свободы и уровня значимости 0.05
    const double critical = 3.841;

    int up = 0;
    int down = 0;

    //сравниваем соседние элементы
    for (int i = 1; i < size; i++)
    {
        if (values[i] >= values[i - 1])
        {
            up++;
        }
        else
        {
            down++;
        }
    }

    //ожидаем примерно одинаковое количество подъемов и спадов
    double expected = static_cast<double>(up + down) / 2.0;

    double chi = 0.0;

    chi += (up - expected) * (up - expected) / expected;
    chi += (down - expected) * (down - expected) / expected;

    return ChiSquareResult{ chi, critical, chi < critical };
}