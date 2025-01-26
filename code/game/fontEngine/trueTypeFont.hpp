#pragma once
#include "platform/platform.hpp"

namespace TrueTypeImplementation
{

#pragma pack(push, 1)
    struct Fixed
    {
        uint16_t integerPart;
        uint16_t pointPart;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct Directory
    {
        uint32_t scalerType;
        uint16_t numTables;
        uint16_t searchRange;
        uint16_t entrySelector;
        uint16_t rangeShift;

        void log()
        {
            LOG("Directory table\n");
            LOG("scalerType: %u, numTables: %u, searchRange: %u, entrySelector: %u, rangeShift %u\n", scalerType, numTables, searchRange, entrySelector, rangeShift);
        }
    };
#pragma pack(pop)

    Directory *ParseDirectoryTable(uint8_t *rawBytes)
    {
        RB_32B_FROM_BIG_ENDIAN(rawBytes);
        RB_16B_FROM_BIG_ENDIAN(rawBytes + 4);
        RB_16B_FROM_BIG_ENDIAN(rawBytes + 6);
        RB_16B_FROM_BIG_ENDIAN(rawBytes + 8);
        RB_16B_FROM_BIG_ENDIAN(rawBytes + 10);
        return (Directory *)rawBytes;
    }

#pragma pack(push, 1)
    struct Offset
    {
        uint32_t tag;
        uint32_t checksum;
        uint32_t offset;
        uint32_t length;

        void log()
        {
            LOG("tag: \"");
            char *tmp = (char *)&tag;
            for (int i = 0; i < 4; ++i)
            {
                LOG("%c", tmp[i]);
            }
            LOG("\" checksum: %u, offset: %u, length: %u\n", checksum, offset, length);
        }
    };
#pragma pack(pop)

    Offset *ParseOffsetsTable(uint8_t *rawBytes, uint16_t numTables)
    {
        uint8_t *it = rawBytes;
        for (uint16_t i = 0; i < numTables; ++i)
        {
            RB_32B_FROM_BIG_ENDIAN(it + 4);
            RB_32B_FROM_BIG_ENDIAN(it + 8);
            RB_32B_FROM_BIG_ENDIAN(it + 12);
            it += 16;
        }
        return (Offset *)rawBytes;
    }

#pragma pack(push, 1)
    struct TableHead
    {
        Fixed version;
        Fixed fontRevision;
        uint32_t checkSumAdjustment;
        uint32_t magicNumber;
        uint16_t flags;
        uint16_t unitsPerEm;
        int64_t created;
        int64_t modified;
        int16_t xMin;
        int16_t yMin;
        int16_t xMax;
        int16_t yMax;
        uint16_t macStyle;
        uint16_t lowestRecPPEM;
        int16_t fontDirectionHint;
        int16_t indexToLocFormat;
        int16_t glyphDataFormat;

        void log()
        {
            LOG("\nHead table\n");
            LOG("version: %u.%u fontRevision: %u.%u checkSumAdjustment: %u magicNumber %u\n", version.integerPart, version.pointPart, fontRevision.integerPart, fontRevision.pointPart, checkSumAdjustment, magicNumber);
            LOG("flags: %u unitsPerEm: %u created %lld modified %lld xMin %d yMin %d xMax %d yMax %d\n", flags, unitsPerEm, created, modified, xMin, yMin, xMax, yMax);
            LOG("macStyle: %u lowestRecPPEM: %u fontDirectionHint: %d indexToLocFormat %d glyphDataFormat %d\n", macStyle, lowestRecPPEM, fontDirectionHint, indexToLocFormat, glyphDataFormat);
        }
    };
#pragma pack(pop)

    TableHead *ParseHeadTable(uint8_t *table, uint32_t length)
    {
        RB_16B_FROM_BIG_ENDIAN(table);      // version
        RB_16B_FROM_BIG_ENDIAN(table + 2);  // version
        RB_16B_FROM_BIG_ENDIAN(table + 4);  // fontRevision
        RB_16B_FROM_BIG_ENDIAN(table + 6);  // fontRevision
        RB_32B_FROM_BIG_ENDIAN(table + 8);  // checkSumAdjustment
        RB_32B_FROM_BIG_ENDIAN(table + 12); // magicNumber
        RB_16B_FROM_BIG_ENDIAN(table + 16); // flags
        RB_16B_FROM_BIG_ENDIAN(table + 18); // unitsPerEm
        RB_64B_FROM_BIG_ENDIAN(table + 20); // created
        RB_64B_FROM_BIG_ENDIAN(table + 28); // modified
        RB_16B_FROM_BIG_ENDIAN(table + 36); // xMin
        RB_16B_FROM_BIG_ENDIAN(table + 38); // yMin
        RB_16B_FROM_BIG_ENDIAN(table + 40); // xMax
        RB_16B_FROM_BIG_ENDIAN(table + 42); // yMax
        RB_16B_FROM_BIG_ENDIAN(table + 44); // macStyle
        RB_16B_FROM_BIG_ENDIAN(table + 46); // lowestRecPPEM
        RB_16B_FROM_BIG_ENDIAN(table + 48); // fontDirectionHint
        RB_16B_FROM_BIG_ENDIAN(table + 50); // indexToLocFormat
        RB_16B_FROM_BIG_ENDIAN(table + 52); // glyphDataFormat
        return (TableHead *)table;
    }

/// 'OS/2' table
#pragma pack(push, 1)
    struct TableOS2
    {
        uint16_t version;
        int16_t xAvgCharWidth;
        uint16_t usWeightClass;
        uint16_t usWidthClass;
        int16_t fsType;
        int16_t ySubscriptXSize;
        int16_t ySubscriptYSize;
        int16_t ySubscriptXOffset;
        int16_t ySubscriptYOffset;
        int16_t ySuperscriptXSize;
        int16_t ySuperscriptYSize;
        int16_t ySuperscriptXOffset;
        int16_t ySuperscriptYOffset;
        int16_t yStrikeoutSize;
        int16_t yStrikeoutPosition;
        int16_t sFamilyClass;
        uint8_t panose[10];
        uint32_t ulUnicodeRange[4];
        uint8_t achVendID[4];
        uint16_t fsSelection;
        uint16_t fsFirstCharIndex;
        uint16_t fsLastCharIndex;

        // Version 1
        int16_t sTypoAscender;
        int16_t sTypoDescender;
        int16_t sTypoLineGap;
        uint16_t usWinAscent;
        uint16_t usWinDescent;

        // Version 2
        uint32_t ulCodePageRange1;
        uint32_t ulCodePageRange2;

        // Version 3 & 4
        int16_t sxHeight;
        int16_t sCapHeight;
        uint16_t usDefaultChar;
        uint16_t usBreakChar;
        uint16_t usMaxContext;

        // Version 5
        uint16_t usLowerPointSize;
        uint16_t usUpperPointSize;

        void log(uint32_t length)
        {
            LOG("\n");
            if (length < 78)
            {
                LOG("OS/2 table version 0\n");
            }
            else if (length < 86)
            {
                LOG("OS/2 table version 1\n");
            }
            else if (length < 96)
            {
                LOG("OS/2 table version 2\n");
            }
            else if (length < 100)
            {
                LOG("OS/2 table version 3 or 4\n");
            }
            else
            {
                LOG("OS/2 table version 5\n");
            }

            LOG("version: %u, xAvgCharWidth: %d, usWeightClass: %u, usWidthClass: %u\n", version, xAvgCharWidth, usWeightClass, usWidthClass);
            LOG("fsType %d, ySubscriptXSize: %d, ySubscriptYSize: %d, ySubscriptXOffset %d, ySubscriptYOffset %d\n", fsType, ySubscriptXSize, ySubscriptYSize, ySubscriptXOffset, ySubscriptYOffset);
            LOG("ySuperscriptXSize %d, ySuperscriptYSize %d ySuperscriptXOffset %d ySuperscriptYOffset %d\n", ySuperscriptXSize, ySuperscriptYSize, ySuperscriptXOffset, ySuperscriptYOffset);
            LOG("yStrikeoutSize %d, yStrikeoutPosition %d, sFamilyClass %d\n", yStrikeoutSize, yStrikeoutPosition, sFamilyClass);
            LOG("panose: [");
            for (int i = 0; i < 10; ++i)
            {
                LOG("%u", panose[i]);
                if (i != 9)
                {
                    LOG(",");
                }
            }
            LOG("]\n");
            LOG("ulUnicodeRange: [");
            for (int i = 0; i < 4; ++i)
            {
                LOG("%u", ulUnicodeRange[i]);
                if (i != 3)
                {
                    LOG(",");
                }
            }
            LOG("]\n");

            LOG("achVendID: [");
            for (int i = 0; i < 4; ++i)
            {
                LOG("%u", achVendID[i]);
                if (i != 3)
                {
                    LOG(",");
                }
            }
            LOG("]\n");
            LOG("fsSelection %u, fsFirstCharIndex %u, fsLastCharIndex %u\n", fsSelection, fsFirstCharIndex, fsLastCharIndex);

            if (length < 78)
            {
                return;
            }
            LOG("sTypoAscender %d, sTypoDescender %d, sTypoLineGap %d, usWinAscent %u, usWinDescent %u\n", sTypoAscender, sTypoDescender, sTypoLineGap, usWinAscent, usWinDescent);

            if (length < 86)
            {
                return;
            }
            LOG("ulCodePageRange1 %u, ulCodePageRange2 %u\n", ulCodePageRange1, ulCodePageRange2);
            if (length < 96)
            {
                return;
            }
            LOG("sxHeight %d, sCapHeight %d, usDefaultChar %u, usBreakChar %u, usMaxContext %u\n", sxHeight, sCapHeight, usDefaultChar, usBreakChar, usMaxContext);
            if (length < 100)
            {
                return;
            }
            LOG("usLowerPointSize %u, usUpperPointSize %u\n", usLowerPointSize, usUpperPointSize);
        }
    };
#pragma pack(pop)

    TableOS2 *parseOS2Table(uint8_t *table, uint32_t length)
    {
        if (length < 68)
        {
            LOG("Invalid OS/2 table length!\n");
            return nullptr;
        }
        TableOS2 *result = (TableOS2 *)table;
        RB_16B_FROM_BIG_ENDIAN(table);      // version
        RB_16B_FROM_BIG_ENDIAN(table + 2);  // xAvgCharWidth
        RB_16B_FROM_BIG_ENDIAN(table + 4);  // usWeightClass
        RB_16B_FROM_BIG_ENDIAN(table + 6);  // usWidthClass
        RB_16B_FROM_BIG_ENDIAN(table + 8);  // fsType
        RB_16B_FROM_BIG_ENDIAN(table + 10); // ySubscriptXSize
        RB_16B_FROM_BIG_ENDIAN(table + 12); // ySubscriptYSize
        RB_16B_FROM_BIG_ENDIAN(table + 14); // ySubscriptXOffset
        RB_16B_FROM_BIG_ENDIAN(table + 16); // ySubscriptYOffset
        RB_16B_FROM_BIG_ENDIAN(table + 18); // ySuperscriptXSize
        RB_16B_FROM_BIG_ENDIAN(table + 20); // ySuperscriptYSize
        RB_16B_FROM_BIG_ENDIAN(table + 22); // ySuperscriptXOffset
        RB_16B_FROM_BIG_ENDIAN(table + 24); // ySuperscriptYOffset
        RB_16B_FROM_BIG_ENDIAN(table + 26); // yStrikeoutSize
        RB_16B_FROM_BIG_ENDIAN(table + 28); // yStrikeoutPosition
        RB_16B_FROM_BIG_ENDIAN(table + 30); // sFamilyClass
                                            // panose
        RB_32B_FROM_BIG_ENDIAN(table + 42); // ulUnicodeRange[0]
        RB_32B_FROM_BIG_ENDIAN(table + 46); // ulUnicodeRange[1]
        RB_32B_FROM_BIG_ENDIAN(table + 50); // ulUnicodeRange[2]
        RB_32B_FROM_BIG_ENDIAN(table + 54); // ulUnicodeRange[3]
                                            // achVendID
        RB_16B_FROM_BIG_ENDIAN(table + 62); // fsSelection
        RB_16B_FROM_BIG_ENDIAN(table + 64); // fsFirstCharIndex
        RB_16B_FROM_BIG_ENDIAN(table + 66); // fsLastCharIndex

        if (length < 78)
        {
            return result;
        }

        // Version 1
        RB_16B_FROM_BIG_ENDIAN(table + 68); // sTypoAscender
        RB_16B_FROM_BIG_ENDIAN(table + 70); // sTypoDescender
        RB_16B_FROM_BIG_ENDIAN(table + 72); // sTypoLineGap
        RB_16B_FROM_BIG_ENDIAN(table + 74); // usWinAscent
        RB_16B_FROM_BIG_ENDIAN(table + 76); // usWinDescent

        if (length < 86)
        {
            return result;
        }

        // Version 2
        RB_32B_FROM_BIG_ENDIAN(table + 78); // ulCodePageRange1
        RB_32B_FROM_BIG_ENDIAN(table + 82); // ulCodePageRange2

        if (length < 96)
        {
            return result;
        }

        RB_16B_FROM_BIG_ENDIAN(table + 86); // sxHeight;
        RB_16B_FROM_BIG_ENDIAN(table + 88); // sCapHeight;
        RB_16B_FROM_BIG_ENDIAN(table + 90); // usDefaultChar;
        RB_16B_FROM_BIG_ENDIAN(table + 92); // usBreakChar;
        RB_16B_FROM_BIG_ENDIAN(table + 94); // usMaxContext;

        if (length < 100)
        {
            return result;
        }
        RB_16B_FROM_BIG_ENDIAN(table + 96); // usLowerPointSize;
        RB_16B_FROM_BIG_ENDIAN(table + 98); // usUpperPointSize;

        return result;
    }

#pragma pack(push, 1)
    struct TableSimpleGlyph
    {
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct TableCompoundGlyph
    {
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct TableGlyph
    {
        int16_t numberOfContours;
        int16_t xMin;
        int16_t yMin;
        int16_t xMax;
        int16_t yMax;
    };
#pragma pack(pop)
}

#define TTF_TAG(tag) *((uint32_t *)tag)

#define TTFontTable(fontFile, index) (((uint8_t *)fontFile.directoryTable) + fontFile.offsetsTable[index].offset), fontFile.offsetsTable[index].length

struct TrueTypeFontFile
{
    TrueTypeImplementation::Directory *directoryTable;
    TrueTypeImplementation::Offset *offsetsTable;
    TrueTypeImplementation::TableHead *headTable;
    TrueTypeImplementation::TableOS2 *os2Table;

    void log(uint32_t os2TableLength)
    {
        directoryTable->log();
        for (uint16_t i = 0; i < directoryTable->numTables; ++i)
        {
            offsetsTable[i].log();
        }
        headTable->log();
        os2Table->log(os2TableLength);
    }
};

namespace trueTypeFontinternal
{

#if TTF_CHECK_OFFSETS
    uint32_t CalculateChecksum(uint8_t *table, uint32_t length)
    {
        uint32_t checksum = 0;
        uint32_t value;
        uint8_t valueBytes[4];

        const uint32_t iterations = (length + 3) / 4;
        for (uint32_t i = 0; i < iterations; ++i)
        {
            valueBytes[0] = *table++;
            valueBytes[1] = *table++;
            valueBytes[2] = *table++;
            valueBytes[3] = *table++;
            RB_32B_FROM_BIG_ENDIAN(valueBytes);

            checksum += *((uint32_t *)valueBytes);
        }
        return checksum;
    }
#endif

}

void LoadFont(const char *path, size_t nameSize, PlatformLayer *platformLayer)
{

    FileManagerFileHandle fileHandle;
    TrueTypeFontFile fontFile;

    platformLayer->fileManager.OpenFileAndGetSize(path, nameSize, &fileHandle);
    uint8_t *rawData = platformLayer->assetsMemory.push(fileHandle.size);
    platformLayer->fileManager.ReadFileAndClose(fileHandle, rawData);

    fontFile.directoryTable = TrueTypeImplementation::ParseDirectoryTable(rawData);
    rawData += sizeof(TrueTypeImplementation::Directory);

    fontFile.offsetsTable = TrueTypeImplementation::ParseOffsetsTable(rawData, fontFile.directoryTable->numTables);

    uint16_t os2TableLength = 0;

    for (uint16_t i = 0; i < fontFile.directoryTable->numTables; ++i)
    {
        if (fontFile.offsetsTable[i].tag == TTF_TAG("head"))
        {
            fontFile.headTable = TrueTypeImplementation::ParseHeadTable(TTFontTable(fontFile, i));
        }
        else if (fontFile.offsetsTable[i].tag == TTF_TAG("OS/2"))
        {
            os2TableLength = fontFile.offsetsTable[i].length;
            fontFile.os2Table = TrueTypeImplementation::parseOS2Table(TTFontTable(fontFile, i));
        }
    }
    fontFile.log(os2TableLength);
   
    crash();
}