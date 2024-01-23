#include "./ImageDriver.h"

#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include "./Font.h"
#include "./JetBrains.h"
#include "esp_log.h"

#define TAG "IMAGEDRIVER"

ImageDriver::ImageDriver(size_t width, size_t height)
    : mWidth(width),
      mInternalWidth(width / elementSize),
      mHeight(height),
      mImgSize(mInternalWidth * height),
      mImg(mImgSize, 0) {}

void ImageDriver::drawGraph(Vec2u min, Vec2u end,
                            std::span<const Vec2f> points) {
    // TODO: Implement
}

void ImageDriver::drawImage(Vec2u coord, const Image &image) {
    // image => Übergebenes Element
    // display => Bildschirm speicher
    auto shift = coord.x % elementSize;  // Amount to shift to right

    for (int imgY = 0; imgY < image.height(); imgY++) {
        for (int imgX = 0; imgX < image.byteWidth(); imgX++) {
            size_t displayPos =
                coord2index({coord.x + imgX * elementSize, coord.y + imgY})
                    .first;
            size_t imgPos = imgY * image.byteWidth() + imgX;
            mImg[displayPos] |= image.data()[imgPos] >> shift;
            if ((displayPos + 1) % mInternalWidth ==
                0) {  // Reached right end of Display
                break;
            }
            mImg[displayPos + 1] |= image.data()[imgPos]
                                    << (elementSize - shift);
        }
        if (imgY + coord.y + 1 == mHeight) {  // Reached bottom end of display
            break;
        }
    }
}

void ImageDriver::drawLine(Vec2u from, Vec2u to) {
    // Bresenham algorithm (gradient <= 1)
    const Vec2i d = static_cast<Vec2i>(to) - static_cast<Vec2i>(from);
    int D = (d.y + d.y) - d.x;
    unsigned y = from.y;

    for (unsigned x = from.x; x <= to.x; x++) {
        drawPoint({x, y});
        if (D > 0) {
            y += 1;
            D = D - (d.x + d.x);
        }
        D += d.y + d.y;
    }
}

void ImageDriver::drawVerticalLine(Vec2u pos, Element mask, size_t height) {
    for (size_t i = 0; i < height; i++) {
        mImg[coord2index({pos.x, pos.y + i}).first] |= mask;
    }
}

void ImageDriver::drawFilledRect(Vec2u pos, Vec2u size) {
    if (pos.x >= mWidth || pos.y >= mHeight) {
        return;
    }
    unsigned width = std::min(mWidth - pos.x, size.x);
    unsigned height = std::min(mHeight - pos.y, size.y);

    unsigned leftOffset = pos.x % elementSize;
    unsigned rightOffset = elementSize - ((pos.x + width) % elementSize);

    Element startMask = 0xffu >> leftOffset;
    Element endMask = 0xffu << rightOffset;

    if (pos.x / elementSize == (pos.x + size.x) / elementSize) {
        // Alles in einem Byte
        drawVerticalLine(pos, startMask & endMask, height);
        return;
    }
    drawVerticalLine(pos, startMask, height);
    width -= elementSize - leftOffset;

    while (width >= 8) {
        pos.x += 8;
        drawVerticalLine(pos, 0xff, height);
        width -= 8;
    }
    pos.x += 8;
    drawVerticalLine(pos, endMask, height);
}

void ImageDriver::drawPoint(Vec2u coord) {
    if (const auto [index, mask] = coord2index(coord); index != SIZE_MAX) {
        mImg[index] |= (static_cast<Element>(1)
                        << ((1u - elementSize) - (coord.x % elementSize)));
    }
}

void ImageDriver::drawText(Vec2u coord, std::string_view text) {
    const Font &font = JetBrains::font16;
    printf("Printing text\n");
    for (auto ch : text) {
        const auto it =
            std::find(font.unicode_list.begin(), font.unicode_list.end(), ch);
        printf("%c \n", ch);
        if (it == font.unicode_list.end()) {
            // std::cerr << "Character" <<  ch << "not found in font" <<
            // std::endl;
            printf("Char not found\n");
            continue;
        }
        const auto index = std::distance(font.unicode_list.begin(), it);
        const auto &dsc = font.glyph_dsc[index];
        printf("Index: %d\n", index);

        // TODO: See if this actually works
        auto glyph = &font.glyph_bitmap[dsc.glyph_index];
        printf("Before Span\n");
        printf("Height: %d\n", font.h_px);
        printf("Width: %d\n", dsc.w_px);
        auto byteSize = (dsc.w_px + elementSize - 1) / elementSize;
        auto size = static_cast<size_t>(byteSize * font.h_px);
        const std::span glyphSpan{glyph, size};
        printf("OwnSize: %zu\n", size);
        printf("Size: %zu\n", glyphSpan.size());
        for (auto c : glyphSpan) {
            printf("%d ", c);
        }
        printf("\n");
        printf("Before Img\n");
        Image img = Image(glyphSpan, dsc.w_px);
        printf("After img init Img\n");
        drawImage(coord, img);
        printf("After Img\n");
        coord.x += dsc.w_px;
    }
}
