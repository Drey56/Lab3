#include "Prng.h"
#include "Statistics.h"
#include "Tests.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

static int toRange(uint32_t value, int minValue, int maxValue)
{
    //переводим число генератора в заданный диапазон
    return minValue + static_cast<int>(value % (maxValue - minValue + 1));
}

static int* generateSample(IGenerator& generator, int sampleSize, int minValue, int maxValue)
{
    //создаем динамический массив для выборки
    int* sample = new int[sampleSize];

    for (int i = 0; i < sampleSize; i++)
    {
        sample[i] = toRange(generator.next(), minValue, maxValue);
    }

    return sample;
}

static void analyzeGenerator(IGenerator& generator, int sampleCount, int sampleSize, int minValue, int maxValue)
{
    cout << "\nгенератор: " << generator.name() << "\n";

    for (int i = 1; i <= sampleCount; i++)
    {
        //получаем выборку через динамический массив
        int* sample = generateSample(generator, sampleSize, minValue, maxValue);

        SampleStats stats = calculateStats(sample, sampleSize);
        ChiSquareResult uniform = chiSquareUniform(sample, sampleSize, minValue, maxValue, 10);
        ChiSquareResult random = chiSquareRandomness(sample, sampleSize);

        cout << "выборка " << setw(2) << i
            << " | среднее: " << setw(10) << fixed << setprecision(2) << stats.mean
            << " | отклонение: " << setw(10) << stats.deviation
            << " | вариация: " << setw(8) << stats.variation
            << " | равномерность: " << (uniform.passed ? "да" : "нет")
            << " | случайность: " << (random.passed ? "да" : "нет")
            << "\n";

        delete[] sample;
    }
}

static void runTestsForGenerator(IGenerator& generator)
{
    cout << "\nтесты для генератора: " << generator.name() << "\n";

    int testCount = 0;

    //запускаем пять тестов для битовой последовательности
    TestResult* results = runFiveTests(generator, 100000, testCount);

    for (int i = 0; i < testCount; i++)
    {
        cout << results[i].name
            << " | значение: " << fixed << setprecision(6) << results[i].value
            << " | результат: " << (results[i].passed ? "пройден" : "не пройден")
            << "\n";
    }

    delete[] results;
}

static long long measureCustomGenerator(IGenerator& generator, int count)
{
    volatile uint32_t sink = 0;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < count; i++)
    {
        sink ^= generator.next();
    }

    auto finish = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(finish - start).count();
}

static long long measureStandardGenerator(int count)
{
    mt19937 generator(12345);
    volatile uint32_t sink = 0;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < count; i++)
    {
        sink ^= generator();
    }

    auto finish = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(finish - start).count();
}

static void speedComparison()
{
    //массив объемов генерации
    int sizesCount = 7;
    int* sizes = new int[sizesCount];

    sizes[0] = 1000;
    sizes[1] = 5000;
    sizes[2] = 10000;
    sizes[3] = 50000;
    sizes[4] = 100000;
    sizes[5] = 500000;
    sizes[6] = 1000000;

    //файл result.txt должен быть создан вручную заранее
    ofstream file("result.txt", ios::app);

    if (!file.is_open())
    {
        cout << "\nне удалось открыть result.txt\n";
        delete[] sizes;
        return;
    }

    file << "\nсравнение скоростей генерации\n";

    cout << "\nзамеры скорости записаны в result.txt\n";

    for (int i = 0; i < sizesCount; i++)
    {
        int size = sizes[i];

        MixedLCG g1(12345);
        XorWeyl g2(12345);
        MultiplyWithCarry g3(12345);

        long long t1 = measureCustomGenerator(g1, size);
        long long t2 = measureCustomGenerator(g2, size);
        long long t3 = measureCustomGenerator(g3, size);
        long long ts = measureStandardGenerator(size);

        file << "\nобъем: " << size << "\n";
        file << "mixed_lcg: " << t1 << " мкс\n";
        file << "xor_weyl: " << t2 << " мкс\n";
        file << "multiply_with_carry: " << t3 << " мкс\n";
        file << "mt19937: " << ts << " мкс\n";

        cout << "объем: " << setw(8) << size
            << " | mixed_lcg: " << setw(8) << t1
            << " | xor_weyl: " << setw(8) << t2
            << " | mwc: " << setw(8) << t3
            << " | mt19937: " << setw(8) << ts
            << " мкс\n";
    }

    file.close();

    delete[] sizes;
}

int main()
{

    setlocale(LC_ALL, "Russian");
    const int sampleCount = 20;
    const int sampleSize = 1000;
    const int minValue = 0;
    const int maxValue = 9999;

    MixedLCG generator1(12345);
    XorWeyl generator2(54321);
    MultiplyWithCarry generator3(98765);

    analyzeGenerator(generator1, sampleCount, sampleSize, minValue, maxValue);
    analyzeGenerator(generator2, sampleCount, sampleSize, minValue, maxValue);
    analyzeGenerator(generator3, sampleCount, sampleSize, minValue, maxValue);

    MixedLCG testGenerator1(12345);
    XorWeyl testGenerator2(54321);
    MultiplyWithCarry testGenerator3(98765);

    runTestsForGenerator(testGenerator1);
    runTestsForGenerator(testGenerator2);
    runTestsForGenerator(testGenerator3);

    speedComparison();

    return 0;
}