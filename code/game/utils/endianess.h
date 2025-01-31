#pragma once

#include <cstdint>

#define ___LITTLE_ENDIAN 0x41424344UL
#define ___BIG_ENDIAN 0x44434241UL
#define ___PDP_ENDIAN 0x42414443UL
#define ___ENDIAN_ORDER ('ABCD')

#if ___ENDIAN_ORDER == ___LITTLE_ENDIAN

uint16_t UInt16FromBigEndian(const uint8_t *bytes)
{
    uint16_t result = bytes[0];
    result <<= 8;
    result += bytes[1];
    return result;
}

int16_t Int16FromBigEndian(const uint8_t *bytes)
{
    int16_t result = bytes[0];
    result <<= 8;
    result += bytes[1];
    return result;
}

uint32_t UInt32FromBigEndian(const uint8_t *bytes)
{
    uint32_t result = bytes[0];
    result <<= 8;
    result += bytes[1];
    result <<= 8;
    result += bytes[2];
    result <<= 8;
    result += bytes[3];
    return result;
}

uint64_t UInt64FromBigEndian(const uint8_t *bytes)
{
    uint64_t result = bytes[0];
    result <<= 8;
    result += bytes[1];
    result <<= 8;
    result += bytes[2];
    result <<= 8;
    result += bytes[3];
    result <<= 8;
    result += bytes[4];
    result <<= 8;
    result += bytes[5];
    result <<= 8;
    result += bytes[6];
    result <<= 8;
    result += bytes[7];
    return result;
}


#else
#error "These architectures even exist?"
#endif