#pragma once

#include <cstdint>
#include <string>

class IGenerator
{
public:
    virtual ~IGenerator() = default;

    virtual uint32_t next() = 0;
    virtual std::string name() const = 0;
};

class MixedLCG : public IGenerator
{
private:
    uint32_t state;

public:
    explicit MixedLCG(uint32_t seed);

    uint32_t next() override;
    std::string name() const override;
};

class XorWeyl : public IGenerator
{
private:
    uint32_t state;
    uint32_t weyl;

public:
    explicit XorWeyl(uint32_t seed);

    uint32_t next() override;
    std::string name() const override;
};

class MultiplyWithCarry : public IGenerator
{
private:
    uint32_t state;
    uint32_t carry;

public:
    explicit MultiplyWithCarry(uint32_t seed);

    uint32_t next() override;
    std::string name() const override;
};