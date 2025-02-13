#pragma once

#include "../platform/platform.hpp"
#include <algorithm> // For std::sort
#include <cmath>     // For std::abs

struct Section {
    float x1, y1, x2, y2;
};

struct Line {
    // x + by - c = 0
    float b, c;
    float x1, x2;
};


struct Hitpoints {

};


void append(float x, float hitPoints[], int *count) {
    hitPoints[*count] = x;
    *count = *count + 1;
}

float intersectionWithSection(float y, float x1, float y1, float x2, float y2) {
    // x1 + B*y1 + C == X2 * B*Y2 + C == 0
    float B = (x1 - x2) / (y2 - y1);
    float C = -x1 - B * y1;
    // x + B*y + C == 0
    return -C - B*y;
}

void addHitPointWithSection(
    float hitPoints[], int *count,
    float y, 
    float *previousSign,
    float x1, float y1, float x2, float y2
) {
    if (y1 == y2) {
        if (y == y1) {
            append(-x2 * *previousSign, hitPoints, count);
        }
        return;
    } else if (y1 < y2) {
        // pointing up
        *previousSign = 1;
        if (y1 < y && y <= y2) {
            append(intersectionWithSection(y, x1, y1, x2, y2), hitPoints, count);
        }
    } else if (y1 > y2) {
        *previousSign = -1;
        if (y2 < y && y <= y1) {
            append(-intersectionWithSection(y, x1, y1, x2, y2), hitPoints, count);
        }
    }
}



void drawGlyph(Section glyphs[], int arrowCount[], int glyphCount, int8_t bitmap[], int bitmapWidth, int bitmapHeight, uint8_t *someMemory) {
    float y = 0.5;
    float x;
    int counter;
    int8_t *row = bitmap;
    int intersections;
    int contourStart;
    int contourIndex;
    float *tmpArray = (float*) someMemory;

    int firstHandled;
    int contourStartIdx;
    float previousSign;

    int inersectionI;

    for (int i = 0; i < bitmapHeight; i++) {
        intersections = 0;
        contourStart = 0;
        contourIndex = 0;
        contourStartIdx = 0;
        while (contourIndex < glyphCount) {
            firstHandled = contourStartIdx;
            while (glyphs[firstHandled].y1 == glyphs[firstHandled].y2) {
                firstHandled += 1;
            }

            if (glyphs[firstHandled].y1 < glyphs[firstHandled].y2) {
                previousSign = 1.0;
            } else {
                previousSign = -1.0;
            }
            firstHandled += 1;

            for (int a = firstHandled; a < arrowCount[contourIndex]; a++) {
                addHitPointWithSection(
                    tmpArray, &intersections,
                    y, &previousSign,
                    glyphs[a].x1, glyphs[a].y1, glyphs[a].x2, glyphs[a].y2
                );
            }
            for (int a = contourStartIdx; a < firstHandled; a++) {
                addHitPointWithSection(
                    tmpArray, &intersections,
                    y, &previousSign,
                    glyphs[a].x1, glyphs[a].y1, glyphs[a].x2, glyphs[a].y2
                );
            }
            contourStartIdx = arrowCount[contourIndex];
            contourIndex++;
        }

        std::sort(tmpArray, tmpArray + intersections, [](float lhs, float rhs) {
           return std::abs(lhs) < std::abs(rhs);
        });

        LOG("y%d :: ", i);
        for (int t = 0; t < intersections; ++t) {
            LOG("%f ", tmpArray[t]);
        }
        LOG("\n");

        counter = 0;
        x = 0.5;
        inersectionI = 0;


        for (int j = 0; j < bitmapWidth; ++j) {
            while (inersectionI < intersections && std::abs(tmpArray[inersectionI]) <= x) {
                if (tmpArray[inersectionI] > 0) {
                    counter += 1;
                } else {
                    counter -= 1;
                }
                inersectionI += 1;
            }

            if (counter != 0) {
                row[j] = 255;
            }
            else {
                row[j] = 0;
            }
            x += 1;
        }


        y += 1;
        row += bitmapWidth;
    }

    // crash();
}

#if TESTS

#endif