#include "Prng.h"

MixedLCG::MixedLCG(uint32_t seed)
{
    //начальное значение не должно быть нулевым
    state = seed == 0 ? 1 : seed;
}

uint32_t MixedLCG::next()
{
    //выполняем линейный конгруэнтный шаг
    state = state * 1664525u + 1013904223u;

    //дополнительно перемешиваем биты
    uint32_t x = state;
    x ^= x >> 15;
    x *= 2246822519u;
    x ^= x >> 13;

    return x;
}

std::string MixedLCG::name() const
{
    return "mixed_lcg";
}

XorWeyl::XorWeyl(uint32_t seed)
{
    //задаем начальное состояние xorshift
    state = seed == 0 ? 2463534242u : seed;

    //задаем начальное значение последовательности вейля
    weyl = seed ^ 0x9E3779B9u;
}

uint32_t XorWeyl::next()
{
    //выполняем перемешивание сдвигами
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    //добавляем последовательность вейля
    weyl += 0x61C88647u;

    return state + weyl;
}

std::string XorWeyl::name() const
{
    return "xor_weyl";
}

MultiplyWithCarry::MultiplyWithCarry(uint32_t seed)
{
    //задаем основное состояние
    state = seed == 0 ? 123456789u : seed;

    //задаем перенос
    carry = (seed >> 16) + 1u;
}

uint32_t MultiplyWithCarry::next()
{
    //выполняем умножение с переносом
    uint64_t t = 4294957665ull * state + carry;

    //сохраняем младшие 32 бита
    state = static_cast<uint32_t>(t);

    //сохраняем старшие 32 бита как перенос
    carry = static_cast<uint32_t>(t >> 32);

    //перемешиваем результат
    uint32_t x = state ^ carry;
    x ^= x >> 16;
    x *= 0x7FEB352Du;
    x ^= x >> 15;

    return x;
}

std::string MultiplyWithCarry::name() const
{
    return "multiply_with_carry";
}