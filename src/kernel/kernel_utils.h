#pragma once

#include <cstdint>

/**
 * @brief Provide some common functions using kernel privliges
 * @note Kernel requires physical memory addresses. Virtual addresses can be converted via `kernel::physical` or `OSEffectiveToPhysical`.
 */
namespace kernel
{
    /**
     * @brief Copy data using kernel privliges
     *
     * Build after: https://en.cppreference.com/w/cpp/string/byte/memcpy
     *
     * @note Kernel requires physical memory addresses
     *
     * @param dst pointer to destination
     * @param src pointer to source
     * @param count size of memory to copy
     */
    void* memcpy(void* dst, const void* src, const uint32_t count);

    /**
     * @brief Compare data using kernel privliges
     *
     * Build after: https://en.cppreference.com/w/cpp/string/byte/memcmp
     *
     * @note Kernel requires physical memory addresses
     *
     * @param lhs pointer to memory to comparison
     * @param rhs pointer to memory to comparison
     * @param count size of memory to compare
     */
    int memcmp(const void* lhs, const void* rhs, uint32_t count);

    /**
     * @brief Simple wrapper to convert virtual into physical addresses
     *
     * @param pointer virtual address pointer
     * @return physical address
     */
    void* physical(const void* pointer);

    /**
     * @brief Simple wrapper to convert virtual into physical addresses
     *
     * @param pointer virtual address
     * @return physical address
     */
    uint32_t physical(const uint32_t address);

};
