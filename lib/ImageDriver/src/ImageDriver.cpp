#include <algorithm>
#include <cstdlib>
#include <string_view>
#include <iostream>

#include "./ImageDriver.h"
#include "./Font.h"
#include "./JetBrains.h"

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
    size_t endX = std::min(coord.x + image.internalWidth(), mWidth);
    size_t endY = std::min(coord.y + image.internalHeight(), mHeight);
    // Pre-calculate shift amount for the elements from the image
    uint8_t shift = coord.x % elementSize;
    for (size_t y = coord.y; y < endY; ++y) {
        size_t sourceY = y - coord.y;
        size_t destIndex = y * mInternalWidth + coord.x / elementSize;
        size_t sourceIndex = sourceY * image.internalWidth();
        for (size_t x = coord.x; x < endX; x += elementSize) {
            Element imgElement = image.data()[sourceIndex];
            // Shift element if necessary
            if (shift != 0 && x + elementSize < endX) {
                imgElement <<= shift;
                // merge with next element
                imgElement |=
                    image.data()[sourceIndex + 1] >> (elementSize - shift);
            }
            mImg[destIndex++] |= imgElement;
            sourceIndex++;
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

void ImageDriver::drawFilledRect(Vec2u pos, Vec2u size) {
    size_t endX = pos.x + size.x;
    if (endX > mWidth) {
        endX = mWidth;
    }

    // Pre-calculate start and end masks and indices
    size_t startIndex = pos.x / elementSize;                   // round down
    size_t endIndex = (endX + elementSize - 1) / elementSize;  // round up

    Element startMask = ~static_cast<Element>(0) << (pos.x % elementSize);
    Element endMask =
        ~static_cast<Element>(0) >> (elementSize - endX % elementSize);
    if (startIndex == endIndex) {
        /*
         * Special case: rect fits in one element
         * Example:
         * - Element: uint8_t
         * - elementSize: 8
         * - pos: {2, 0}
         * - size: {4, 1}
         * - startMask: 0b11111100 (little endian)
         * - endMask:   0b00111111 (little endian)
         * -> This would result in 0b11111111, which is wrong
         * => We need to take the intersection of startMask and endMask.
         *    `endMask` then has no effect.
         */
        startMask &= endMask;
        endMask = 0;
    }
    for (size_t y = pos.y; y < pos.y + size.y && y < mHeight; ++y) {
        size_t yIndex = y * mInternalWidth;

        // Apply start mask
        mImg[yIndex + startIndex] |= startMask;

        // Batch set complete elements
        for (size_t index = startIndex + 1; index < endIndex; ++index) {
            mImg[yIndex + index] = ~static_cast<Element>(0);
        }

        // Apply end mask
        mImg[yIndex + endIndex - 1] |= endMask;
    }
}

void ImageDriver::drawPoint(Vec2u coord) {
    if (const auto [index, mask] = cords2index(coord); index != SIZE_MAX) {
        mImg[index] |= (static_cast<Element>(1)
                        << ((1u - elementSize) - (coord.x % elementSize)));
    }
}

void ImageDriver::drawText(Vec2u coord, std::string_view text) {
    const Font &font = JetBrains::font16;
    for (auto ch : text) {
        const auto it =
            std::find(font.unicode_list.begin(), font.unicode_list.end(), ch);
        if (it == font.unicode_list.end()) {
            std::cerr << "Character" <<  ch << "not found in font" << std::endl;
            continue;
        }
        const auto index = *it;
        const auto &dsc = font.glyph_dsc[index];
        // TODO: See if this actually works
        auto glyph = &font.glyph_bitmap[dsc.glyph_index];
        const std::span glyphSpan{glyph, static_cast<size_t>(dsc.w_px * font.h_px)};
        drawImage(coord, Image(glyphSpan, dsc.w_px));
        coord.x += dsc.w_px;
    }
}
