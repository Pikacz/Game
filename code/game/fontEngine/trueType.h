#pragma once

#include "../utils/endianess.h"
#include "../platform/platform.hpp"

#include "stb_truetype.h"

namespace TTTableDirectory
{
    namespace TableOffset
    {
        constexpr size_t size = 4 + 4 + 4 + 4;

        bool IsTag(const uint8_t *data, const char *tag)
        {
            return data[0] == tag[0] && data[1] == tag[1] && data[2] == tag[2] && data[3] == tag[3];
        }

        uint32_t checksum(const uint8_t *data)
        {
            return UInt32FromBigEndian(data + 4);
        }

        uint32_t offset(const uint8_t *data)
        {
            return UInt32FromBigEndian(data + 8);
        }

        uint32_t length(const uint8_t *data)
        {
            return UInt32FromBigEndian(data + 12);
        }

        void Log(const uint8_t *data)
        {
            LOG("Table: \"%c%c%c%c\"\n", (char)data[0], (char)data[1], (char)data[2], (char)data[3]);
            LOG("  checksum: %u\n", checksum(data));
            LOG("  offset: %u\n", offset(data));
            LOG("  length: %u\n", length(data));
        }
    };

    constexpr size_t directoryTableSize = 4 + 2 * 4;

    uint32_t scalerType(const uint8_t *data)
    {
        return UInt32FromBigEndian(data);
    }

    uint16_t numTables(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 4);
    }

    uint16_t searchRange(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 6);
    }

    uint16_t entrySelector(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 8);
    }

    uint16_t rangeShift(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 10);
    }

    size_t size(const uint8_t *data)
    {
        return directoryTableSize + numTables(data) * TableOffset::size;
    }

    bool checkRanges(const uint8_t *data, size_t availableSize)
    {
        return availableSize >= size(data);
    }

    void Log(const uint8_t *data)
    {
        LOG("Directory table\n");
        LOG("  scalerType: %u\n", scalerType(data));
        LOG("  numTables: %u\n", numTables(data));
        LOG("  searchRange: %u\n", searchRange(data));
        LOG("  entrySelector: %u\n", entrySelector(data));
        LOG("  rangeShift: %u\n", rangeShift(data));
        LOG("\n");

        size_t offset = directoryTableSize;
        for (uint16_t i = 0; i < numTables(data); ++i)
        {
            TableOffset::Log(data + offset);
            offset += TableOffset::size;
        }
    }
};

struct TTFixed
{
    uint16_t intPart;
    uint16_t decPart;

    static TTFixed FromBigEndian(const uint8_t *data)
    {
        return {
            UInt16FromBigEndian(data),
            UInt16FromBigEndian(data + 2)};
    }
};

namespace TTTableHead
{
    TTFixed version(const uint8_t *data)
    {
        return TTFixed::FromBigEndian(data);
    }

    TTFixed fontRevision(const uint8_t *data)
    {
        return TTFixed::FromBigEndian(data + 4);
    }

    uint32_t checkSumAdjustment(const uint8_t *data)
    {
        return UInt32FromBigEndian(data + 8);
    }

    uint32_t magicNumber(const uint8_t *data)
    {
        return UInt32FromBigEndian(data + 12);
    }

    uint16_t flags(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 16);
    }

    uint16_t unitsPerEm(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 18);
    }

    uint64_t created(const uint8_t *data)
    {
        return UInt64FromBigEndian(data + 20);
    }

    uint64_t modified(const uint8_t *data)
    {
        return UInt64FromBigEndian(data + 28);
    }

    int16_t xMin(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 36);
    }

    int16_t yMin(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 38);
    }

    int16_t xMax(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 40);
    }

    int16_t yMax(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 42);
    }

    uint16_t macStyle(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 44);
    }

    uint16_t lowestRecPPEM(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 46);
    }

    int16_t fontDirectionHint(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 48);
    }

    uint16_t indexToLocFormat(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 50);
    }

    int16_t glyphDataFormat(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 52);
    }

    void Log(const uint8_t *data)
    {
        LOG("Head table:\n");
        LOG("  version: %u.%u fontRevision: %u.%u\n", version(data).intPart, version(data).decPart, fontRevision(data).intPart, fontRevision(data).decPart);
        LOG("  checkSumAdjustment %u magicNumber %u flags %u\n", checkSumAdjustment(data), magicNumber(data), flags(data));
        LOG("  unitsPerEm %u created %llu modified %llu\n", unitsPerEm(data), created(data), modified(data));
        LOG("  xMin %d yMin %d xMax %d yMax %d\n", xMin(data), yMin(data), xMax(data), yMax(data));
        LOG("  macStyle %u lowestRecPPEM %u fontDirectionHint %d\n", macStyle(data), lowestRecPPEM(data), fontDirectionHint(data));
        LOG("  indexToLocFormat %u glyphDataFormat %d\n", indexToLocFormat(data), glyphDataFormat(data));
    }
};

namespace TTTableOS2
{
    uint16_t version(const uint8_t *data)
    {
        return UInt16FromBigEndian(data);
    }

    int16_t xAvgCharWidth(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 2);
    }

    uint16_t usWeightClass(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 4);
    }

    uint16_t usWidthClass(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 6);
    }

    int16_t fsType(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 8);
    }

    int16_t ySubscriptXSize(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 10);
    }

    int16_t ySubscriptYSize(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 12);
    }

    int16_t ySubscriptXOffset(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 14);
    }

    int16_t ySubscriptYOffset(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 16);
    }

    int16_t ySuperscriptXSize(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 18);
    }

    int16_t ySuperscriptYSize(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 20);
    }

    int16_t ySuperscriptXOffset(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 22);
    }

    int16_t ySuperscriptYOffset(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 24);
    }

    int16_t yStrikeoutSize(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 26);
    }

    int16_t yStrikeoutPosition(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 28);
    }

    int16_t sFamilyClass(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 30);
    }

    // array of size 10
    uint8_t panose(const uint8_t *data, size_t index)
    {
        return data[32 + index];
    }

    // array of size 4
    uint32_t ulUnicodeRange(const uint8_t *data, size_t index)
    {
        return UInt32FromBigEndian(data + 42 + index * 4);
    }

    // array of size 4
    int8_t achVendID(const uint8_t *data, size_t index)
    {
        return UInt16FromBigEndian(data + 58 + index);
    }

    uint16_t fsSelection(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 62);
    }

    uint16_t fsFirstCharIndex(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 64);
    }

    uint16_t fsLastCharIndex(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 66);
    }

    bool isVersion1(size_t length)
    {
        return length >= 78;
    }

    int16_t sTypoAscender_v1(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 68);
    }

    int16_t sTypoDescender_v1(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 70);
    }

    int16_t sTypoLineGap_v1(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 72);
    }

    uint16_t usWinAscent_v1(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 74);
    }

    uint16_t usWinDescent_v1(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 76);
    }

    bool isVersion2(size_t length)
    {
        return length >= 86;
    }

    uint32_t ulCodePageRange1_v2(const uint8_t *data)
    {
        return UInt32FromBigEndian(data + 78);
    }

    uint32_t ulCodePageRange2_v2(const uint8_t *data)
    {
        return UInt32FromBigEndian(data + 82);
    }

    bool isVersion3Or4(size_t length)
    {
        return length >= 96;
    }

    int16_t sxHeight_v3(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 86);
    }

    int16_t sCapHeight_v3(const uint8_t *data)
    {
        return Int16FromBigEndian(data + 88);
    }

    uint16_t usDefaultChar_v3(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 90);
    }

    uint16_t usBreakChar_v3(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 92);
    }

    uint16_t usMaxContext_v3(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 94);
    }

    bool isVersion5(size_t length)
    {
        return length >= 100;
    }

    uint16_t usLowerPointSize_v5(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 96);
    }

    uint16_t usUpperPointSize_v5(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 98);
    }

    void Log(const uint8_t *data, uint32_t length)
    {
        LOG("Table OS/2\n");
        LOG("  version %u xAvgCharWidth: %d usWeightClass %u usWidthClass %u fsType %d\n", version(data), xAvgCharWidth(data), usWeightClass(data), usWidthClass(data), fsType(data));
        LOG("  ySubscriptXSize %d ySubscriptYSize %d ySubscriptXOffset %d ySubscriptYOffset %d\n", ySubscriptXSize(data), ySubscriptYSize(data), ySubscriptXOffset(data), ySubscriptYOffset(data));
        LOG("  ySuperscriptXSize %d ySuperscriptYSize %d ySuperscriptXOffset %d ySuperscriptYOffset %d\n", ySuperscriptXSize(data), ySuperscriptYSize(data), ySuperscriptXOffset(data), ySuperscriptYOffset(data));
        LOG("  sFamilyClass %d panose [", sFamilyClass(data));
        for (size_t i = 0; i < 9; ++i)
        {
            LOG("%u, ", panose(data, i));
        }
        LOG("%u] ulUnicodeRange [", panose(data, 9));
        for (size_t i = 0; i < 3; ++i)
        {
            LOG("%u, ", ulUnicodeRange(data, i));
        }
        LOG("%u] achVendID [", ulUnicodeRange(data, 3));
        for (size_t i = 0; i < 3; ++i)
        {
            LOG("%u, ", achVendID(data, i));
        }
        LOG("%u]\n", achVendID(data, 3));
        LOG("  fsSelection %u fsFirstCharIndex %u fsLastCharIndex %u\n", fsSelection(data), fsFirstCharIndex(data), fsLastCharIndex(data));

        if (!isVersion1(length))
        {
            return;
        }
        LOG("  sTypoAscender %d sTypoDescender %d sTypoLineGap %d usWinAscent %u usWinDescent %u\n", sTypoAscender_v1(data), sTypoDescender_v1(data), sTypoLineGap_v1(data), usWinAscent_v1(data), usWinDescent_v1(data));
        if (!isVersion2(length))
        {
            return;
        }
        LOG("  ulCodePageRange1 %u ulCodePageRange2 %u\n", ulCodePageRange1_v2(data), ulCodePageRange2_v2(data));
        if (!isVersion3Or4(length))
        {
            return;
        }
        LOG("  sxHeight %d sCapHeight %d usDefaultChar %u usBreakChar %u usMaxContext %u\n", sxHeight_v3(data), sCapHeight_v3(data), usDefaultChar_v3(data), usBreakChar_v3(data), usMaxContext_v3(data));
        if (!isVersion5(length))
        {
            return;
        }
        LOG("  usLowerPointSize %u usUpperPointSize %u\n", usLowerPointSize_v5(data), usUpperPointSize_v5(data));
    }
};

namespace TTTableMaxp
{
    TTFixed version(const uint8_t *data)
    {
        return TTFixed::FromBigEndian(data);
    }

    uint16_t numGlyphs(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 4);
    }

    uint16_t maxPoints(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 6);
    }

    uint16_t maxContours(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 8);
    }

    uint16_t maxComponentPoints(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 10);
    }

    uint16_t maxComponentContours(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 12);
    }

    uint16_t maxZones(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 14);
    }

    uint16_t maxTwilightPoints(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 16);
    }

    uint16_t maxStorage(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 18);
    }

    uint16_t maxFunctionDefs(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 20);
    }

    uint16_t maxInstructionDefs(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 22);
    }

    uint16_t maxStackElements(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 24);
    }

    uint16_t maxSizeOfInstructions(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 26);
    }

    uint16_t maxComponentElements(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 28);
    }

    uint16_t maxComponentDepth(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 30);
    }

    void Log(const uint8_t *data)
    {
        LOG("Table maxp\n");
        LOG("  version %u.%u numGlyph %u maxPoints %u maxContours %u\n", version(data).intPart, version(data).decPart, numGlyphs(data), maxPoints(data), maxContours(data));
        LOG("  maxComponentPoints %u maxComponentContours %u maxZones %u maxTwilightPoints %u\n", maxComponentPoints(data), maxComponentContours(data), maxZones(data), maxTwilightPoints(data));
        LOG("  maxStorage %u maxFunctionDefs %u maxInstructionDefs %u maxStackElements %u\n", maxStorage(data), maxFunctionDefs(data), maxInstructionDefs(data), maxStackElements(data));
        LOG("  maxSizeOfInstructions %u maxComponentElements %u maxComponentDepth %u\n", maxSizeOfInstructions(data), maxComponentElements(data), maxComponentDepth(data));
    }
};

namespace TTTableCMap
{
    uint16_t version(const uint8_t *data)
    {
        return UInt16FromBigEndian(data);
    }

    uint16_t numberSubtables(const uint8_t *data)
    {
        return UInt16FromBigEndian(data + 2);
    }

    uint16_t subtable_platformID(const uint8_t *data, uint16_t subtableIndex)
    {
        return UInt16FromBigEndian(data + 4 + subtableIndex * 8);
    }

    uint16_t subtable_platformSpecificID(const uint8_t *data, uint16_t subtableIndex)
    {
        return UInt16FromBigEndian(data + 4 + subtableIndex * 8 + 2);
    }

    uint32_t subtable_offset(const uint8_t *data, uint16_t subtableIndex)
    {
        return UInt32FromBigEndian(data + 4 + subtableIndex * 8 + 4);
    }

    const uint8_t *DataForTable(const uint8_t *data, uint16_t subtableIndex)
    {
        return data + subtable_offset(data, subtableIndex);
    }

    namespace Data
    {
        uint16_t format(const uint8_t *data)
        {
            return UInt16FromBigEndian(data);
        }

        namespace Format0
        {
            uint16_t length(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 2);
            }

            uint16_t language(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 4);
            }

            uint8_t glyphIndexArray(const uint8_t *data, uint32_t index)
            {
                return data[6 + index];
            }

            void Log(const uint8_t *data)
            {
                LOG("  length %u language %u\n", length(data), language(data));
                constexpr uint16_t inRow = 8;
                constexpr uint16_t inColumn = 256 / inRow;
                for (uint16_t row = 0; row < inColumn; row++)
                {
                    for (uint16_t column = 0; column < inRow; column++)
                    {
                        uint16_t idx = row + column * inColumn;
                        LOG("| %3u :: %-3u ", idx, glyphIndexArray(data, idx));
                    }
                    LOG("|\n");
                }
            }
        };

        namespace Format4
        {

            uint16_t length(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 2);
            }

            uint16_t language(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 4);
            }

            uint16_t segCountX2(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 6);
            }

            uint16_t searchRange(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 8);
            }

            uint16_t entrySelector(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 10);
            }

            uint16_t rangeShift(const uint8_t *data)
            {
                return UInt16FromBigEndian(data + 12);
            }

            uint16_t endCode(const uint8_t *data, uint16_t index, uint16_t segCount)
            {
                return UInt16FromBigEndian(data + 14 + 2 * index);
            }

            uint16_t reservedPad(const uint8_t *data, uint16_t segCount)
            {
                return UInt16FromBigEndian(data + 14 + 2 * segCount);
            }

            uint16_t startCode(const uint8_t *data, uint16_t index, uint16_t segCount)
            {
                return UInt16FromBigEndian(data + 16 + 2 * segCount + 2 * index);
            }

            uint16_t idDelta(const uint8_t *data, uint16_t index, uint16_t segCount)
            {
                return UInt16FromBigEndian(data + 16 + 4 * segCount + 2 * index);
            }

            uint16_t idRangeOffset(const uint8_t *data, uint16_t index, uint16_t segCount)
            {
                return UInt16FromBigEndian(data + 16 + 6 * segCount + 2 * index);
            }

            void Log(const uint8_t *data)
            {
                uint16_t segCount = segCountX2(data) / 2;
                LOG(
                    "  length %u language %u segCountX2 %u searchRange %u entrySelector %u rangeShift %u reservedPad %u\n",
                    length(data), language(data), segCountX2(data), searchRange(data), entrySelector(data), rangeShift(data), reservedPad(data, segCount));
                for (uint16_t i = 0; i < segCount; ++i)
                {
                    LOG(
                        "  %u :: endCode %u startCode %u idDelta %u idRangeOffset %u\n",
                        i, endCode(data, i, segCount), startCode(data, i, segCount), idDelta(data, i, segCount), idRangeOffset(data, i, segCount));
                }
                LOG("Mapping\n");
                uint16_t glyph;
                uint16_t symbol;
                uint16_t startSymbol;
                uint16_t endSymbol;
                uint16_t rangeOffset;
                uint16_t delta;

                for (uint16_t segment = 0; segment < segCount; ++segment)
                {
                    startSymbol = startCode(data, segment, segCount);
                    symbol = startSymbol;
                    endSymbol = endCode(data, segment, segCount);
                    rangeOffset = idRangeOffset(data, segment, segCount);
                    delta = idDelta(data, segment, segCount);
                    while (symbol < 0xffff && symbol <= endSymbol)
                    {
                        if (rangeOffset)
                        {
                            uint16_t tmp = rangeOffset / 2 + (symbol - startSymbol);
                            glyph = idRangeOffset(data, segment + tmp, segCount); // hackly but should work
                        }
                        else
                        {
                            glyph = delta + symbol;
                        }
                        if (glyph)
                        {
                            LOG("%u :: %u, ", symbol, glyph);
                        }
                        symbol += 1;
                    }
                    LOG("\n");
                }
            }
        };

        void Log(const uint8_t *data)
        {
            uint16_t formatVal = format(data);
            LOG("format %u\n", formatVal);
            if (formatVal == 0)
            {
                Format0::Log(data);
            }
            else if (formatVal == 4)
            {
                Format4::Log(data);
            }
            else
            {
                crash();
            }
        }
    };

    void Log(const uint8_t *data)
    {
        LOG("Table cmap");
        uint16_t numberSubtablesVal = numberSubtables(data);
        LOG("  version %u numberSubtables %u\n", version(data), numberSubtablesVal);
        for (uint16_t i = 0; i < numberSubtablesVal; ++i)
        {
            uint32_t offsetVal = subtable_offset(data, i);
            LOG("platformID %u platformSpecificID %u offset %u\n", subtable_platformID(data, i), subtable_platformSpecificID(data, i), offsetVal);
            Data::Log(data + offsetVal);
        }
    }
};

namespace TTTableLoca
{
    uint32_t offset(const uint8_t *data, uint16_t indexToLocFormat, uint16_t index)
    {
        if (indexToLocFormat >= 2)
            return 0;
        if (indexToLocFormat == 0)
        {
            uint32_t result = UInt16FromBigEndian(data + 2 * index);
            result <<= 1;
            return result;
        }
        else
        {
            return UInt32FromBigEndian(data + 4 * index);
        }
    }

    void Log(const uint8_t *data, uint16_t indexToLocFormat, uint16_t numGlyphs)
    {
        LOG("Table loca\n");
        uint32_t current;
        uint32_t next = offset(data, indexToLocFormat, 0);
        for (uint16_t i = 0; i < numGlyphs; ++i)
        {
            current = next;
            next = offset(data, indexToLocFormat, i + 1);
            LOG("%3u :: %5u %5u\n", i, current, next - current);
        }
    }
};

namespace TTTableGlyf
{

    namespace Glyph
    {
        int16_t numberOfContours(const uint8_t *glyph)
        {
            return Int16FromBigEndian(glyph);
        }

        int16_t xMin(const uint8_t *glyph)
        {
            return Int16FromBigEndian(glyph + 2);
        }

        int16_t yMin(const uint8_t *glyph)
        {
            return Int16FromBigEndian(glyph + 4);
        }

        int16_t xMax(const uint8_t *glyph)
        {
            return Int16FromBigEndian(glyph + 6);
        }

        int16_t yMax(const uint8_t *glyph)
        {
            return Int16FromBigEndian(glyph + 8);
        }

        namespace Simple
        {

            void Log(const uint8_t *glyph, int16_t numberOfContours)
            {
                const uint8_t *endPtsOfContours = glyph;
                uint16_t instructionLength = UInt16FromBigEndian(glyph + 2 * numberOfContours);
                LOG("instr Len %u\n", instructionLength);
                const uint8_t *instructions = glyph + 2 * numberOfContours + 2;
                const uint8_t *flags = instructions + instructionLength;
                uint16_t pointsTotal = UInt16FromBigEndian(glyph + 2 * (numberOfContours - 1)) + 1;
                LOG("points total asd: %u\n", pointsTotal);
                uint16_t currentIndex = 0;
                uint16_t totalXLength = 0;
                uint16_t flagsOffset = 0;
                uint16_t xLength;
                uint8_t flag;
                uint16_t repeatCount;
                constexpr uint8_t xShortVector = 1 << 1;
                constexpr uint8_t yShortVector = 1 << 2;
                constexpr uint8_t repeat = 1 << 3;
                constexpr uint8_t xIsSame = 1 << 4;
                constexpr uint8_t yIsSame = 1 << 5;
                // TODO ZABUGOWANE
                uint8_t flagRepeatCount = 0;
                while (currentIndex < pointsTotal)
                {
                    if (flagRepeatCount)
                    {
                        flagRepeatCount--;
                    }
                    else
                    {
                        flag = flags[flagsOffset++];
                        if (flag & repeat)
                        {
                            flagRepeatCount = flags[flagsOffset++];
                        }
                    }

                    if (flag & xShortVector)
                    {
                        totalXLength += 1;
                    }
                    else if (flag & xIsSame)
                    {
                    }
                    else
                    {
                        totalXLength += 2;
                    }

                    currentIndex += 1;
                }
                const uint8_t *xPoints = flags + flagsOffset;
                const uint8_t *yPoints = xPoints + totalXLength;

                currentIndex = 0;
                flagsOffset = 0;
                flagRepeatCount = 0;
                uint16_t xOffset = 0;
                uint16_t yOffset = 0;
                int16_t pointX = 0;
                int16_t pointY = 0;

                int16_t tmp;

                for (int16_t contourIdx = 0; contourIdx < numberOfContours; contourIdx++)
                {
                    LOG("%2d ", contourIdx);
                    pointsTotal = UInt16FromBigEndian(glyph + 2 * contourIdx) + 1;
                    LOG("points for contour: %u\n", pointsTotal);
                    while (currentIndex < pointsTotal)
                    {
                        if (flagRepeatCount)
                        {
                            flagRepeatCount--;
                        }
                        else
                        {
                            flag = flags[flagsOffset++];
                            if (flag & repeat)
                            {
                                flagRepeatCount = flags[flagsOffset++];
                            }
                        }

                        if (flag & xShortVector)
                        {

                            tmp = xPoints[xOffset++];
                            if (flag & xIsSame)
                            {
                                pointX += tmp;
                            }
                            else
                            {
                                pointX -= tmp;
                            }
                        }
                        else
                        {
                            if (flag & xIsSame)
                            {
                            }
                            else
                            {
                                tmp = Int16FromBigEndian(xPoints + xOffset);

                                pointX += tmp;
                                xOffset += 2;
                            }
                        }

                        if (flag & yShortVector)
                        {
                            tmp = yPoints[yOffset++];
                            if (flag & yIsSame)
                            {
                                pointY += tmp;
                            }
                            else
                            {
                                pointY -= tmp;
                            }
                        }
                        else
                        {
                            if (flag & yIsSame)
                            {
                            }
                            else
                            {
                                tmp = Int16FromBigEndian(yPoints + yOffset);
                                pointY += tmp;
                                yOffset += 2;
                            }
                        }
                        LOG("(%5d,%5d) \n", pointX, pointY);

                        currentIndex += 1;
                    }
                    LOG("\n");
                }

                LOG("TODO\n");
            }
        };

        namespace Compound
        {
            void Log(const uint8_t *glyph)
            {
                LOG("TODO\n");
            }
        };

        void Log(const uint8_t *glyph, uint32_t length)
        {
            if (length)
            {
                int16_t contours = numberOfContours(glyph);
                if (contours >= 0)
                {
                    LOG("Simple glyph with %d contours\n", contours);
                }
                else
                {
                    LOG("Compound glyph\n");
                }
                LOG("  xMin %d yMin %d xMax %d yMax %d\n", xMin(glyph), yMin(glyph), xMax(glyph), yMax(glyph));
                if (contours >= 0)
                {
                    Simple::Log(glyph + 10, contours);
                }
                else
                {
                    Compound::Log(glyph);
                }
            }
            else
            {
                LOG("NULL glyph\n");
            }
        }
    };

    void Log(const uint8_t *data, const uint8_t *loca, uint16_t indexToLocFormat, uint16_t numGlyphs)
    {
        LOG("Table glyf\n");
        uint32_t current;
        uint32_t next = TTTableLoca::offset(loca, indexToLocFormat, 0);
        for (uint16_t i = 0; i < 8; i++) // TODO
        {
            current = next;
            next = TTTableLoca::offset(loca, indexToLocFormat, i + 1);
            LOG("%u ", i);
            Glyph::Log(data + current, next - current);
        }
    }
};

struct TrueTypeFontFile
{
    uint8_t *data;
    uint8_t *head;
    uint8_t *os2;
    uint32_t os2Length;
    uint8_t *maxp;
    uint8_t *cmap;
    uint8_t *loca;
    uint8_t *glyf;

    void Log()
    {
        TTTableDirectory::Log(data);
        LOG("\n");
        TTTableHead::Log(head);
        LOG("\n");
        TTTableOS2::Log(os2, os2Length);
        LOG("\n");
        TTTableMaxp::Log(maxp);
        LOG("\n");
        TTTableCMap::Log(cmap);
        LOG("\n");
        uint16_t indexToLocFormat = TTTableHead::indexToLocFormat(head);
        uint16_t numGlyphs = TTTableMaxp::numGlyphs(maxp);
        TTTableLoca::Log(loca, indexToLocFormat, numGlyphs);
        LOG("\n");
        TTTableGlyf::Log(glyf, loca, indexToLocFormat, numGlyphs);
    }
};

void LoadFont(const char *path, size_t nameSize, PlatformLayer *platformLayer)
{
    FileManagerFileHandle fileHandle;
    TrueTypeFontFile fontFile;

    platformLayer->fileManager.OpenFileAndGetSize(path, nameSize, &fileHandle);
    fontFile.data = platformLayer->assetsMemory.push(fileHandle.size);
    platformLayer->fileManager.ReadFileAndClose(fileHandle, fontFile.data);

    ptrdiff_t offset = TTTableDirectory::directoryTableSize;
    for (uint16_t table = 0; table < TTTableDirectory::numTables(fontFile.data); ++table)
    {
        if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "head"))
        {
            fontFile.head = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }
        else if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "OS/2"))
        {
            fontFile.os2Length = TTTableDirectory::TableOffset::length(fontFile.data + offset);
            fontFile.os2 = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }
        else if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "maxp"))
        {
            fontFile.maxp = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }
        else if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "cmap"))
        {
            fontFile.cmap = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }
        else if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "loca"))
        {
            fontFile.loca = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }
        else if (TTTableDirectory::TableOffset::IsTag(fontFile.data + offset, "glyf"))
        {
            fontFile.glyf = fontFile.data + TTTableDirectory::TableOffset::offset(fontFile.data + offset);
        }

        offset += TTTableDirectory::TableOffset::size;
    }

    fontFile.Log();

    platformLayer->assetsMemory.pop(fileHandle.size);

    LOG("STB_TRUE_TYPE\n");
    stbtt_fontinfo costam;
    stbtt_InitFont(&costam, fontFile.data, 0);

    uint16_t asdasd = 0xffff;
    LOG("costam %u\n", asdasd);

    // LOG("STB_TRUE_TYPE %d :: parsed %d\n", 29, stbtt_FindGlyphIndex(&costam, 29));
    // LOG("STB_TRUE_TYPE %d :: parsed %d\n", 32, stbtt_FindGlyphIndex(&costam, 32));
    // LOG("STB_TRUE_TYPE %d :: parsed %d\n", 305, stbtt_FindGlyphIndex(&costam, 305));

    // for (int i = 0; i < TTTableMaxp::numGlyphs(fontFile.maxp); ++i) {
    //     LOG("STB loc of %d == %d\n", i, stbtt__GetGlyfOffset(&costam, i) - 1828);
    // }

    crash();
}