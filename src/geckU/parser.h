#pragma once

#include <cstdint>
#include <format>

class GeckuInstruction
{
public:
    GeckuInstruction* parent;
    GeckuInstruction* right;
    GeckuInstruction* left;

    virtual const char* Debug();
    virtual GeckuInstruction* execute();
};

class RamWrite_8bit : public GeckuInstruction
{
public:
    uint8_t* address;
    uint8_t value;

    const char* Debug() { return std::format("Ram Write: {:#08x} <- {:#02x}", address, value).c_str(); }
};
