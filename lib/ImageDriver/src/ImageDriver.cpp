#include "esp_log.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <string_view>

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
                imgElement >>= shift;
                // merge with next element
                imgElement |=
                    image.data()[sourceIndex + 1] << (elementSize - shift);
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

void ImageDriver::drawVerticalLine(Vec2u pos, Element mask, size_t height){
    for (size_t i = 0; i < height; i++)
    {
        mImg[cords2index({pos.x, pos.y + i}).first] |= mask;
    }
}

void ImageDriver::drawFilledRect(Vec2u pos, Vec2u size) {
    if (pos.x >= mWidth || pos.y >= mHeight) {
        return;
    }
    unsigned width = std::min(mWidth - pos.x, size.x);
    unsigned height = std::min(mHeight - pos.y, size.y);

    unsigned leftOffset = pos.x % elementSize;
    unsigned rightOffset =  elementSize - ((pos.x + width) % elementSize);

    Element startMask = 0xffu >> leftOffset;
    Element endMask = 0xffu << rightOffset;

    if(pos.x / elementSize == (pos.x + size.x) / elementSize){
        // Alles in einem Byte
        drawVerticalLine(pos, startMask&endMask, height);
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
            //std::cerr << "Character" <<  ch << "not found in font" << std::endl;
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
