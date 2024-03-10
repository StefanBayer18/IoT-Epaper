#include "./ImageDriver.h"

#include <algorithm>
#include <bitset>
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

void ImageDriver::drawImage(Vec2u coord, const Image &image) {
    auto shift = coord.x % elementSize;  // Amount to shift to right

    for (int imgY = 0; imgY < image.height(); imgY++) {
        for (int imgX = 0; imgX < image.byteWidth(); imgX++) {
            Vec2u pixelCoord = {coord.x + imgX * elementSize, coord.y + imgY};
            size_t displayIndex = coord2index(pixelCoord).first;
            size_t imgIndex = imgY * image.byteWidth() + imgX;
            mImg[displayIndex] |= image.data()[imgIndex] >> shift;
            // Reached right end of display?
            if ((displayIndex + 1) % mInternalWidth == 0) {
                break;
            }
            mImg[displayIndex + 1] |= image.data()[imgIndex]
                                      << (elementSize - shift);
        }

        // Reached bottom end of display?
        if (imgY + coord.y + 1 == mHeight) {
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
        // All in one element
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
        mImg[index] |= mask;
    }
}

void ImageDriver::drawCenteredText(Vec2u coord, std::string_view text) {
    const Font &font = JetBrains::font24;  // Hardcoded font

    // Measure text width
    int width = 0;
    for (auto ch : text) {
        const auto it =
            std::find(font.unicode_list.begin(), font.unicode_list.end(), ch);
        if (it == font.unicode_list.end()) {
            continue;
        }
        const auto index = std::distance(font.unicode_list.begin(), it);
        const auto &dsc = font.glyph_dsc[index];
        width += dsc.w_px;
    }

    // Draw text at the center by subtracting half of the width
    drawText({coord.x - width / 2, coord.y}, text);
}

void ImageDriver::drawText(Vec2u coord, std::string_view text) {
    const Font &font = JetBrains::font24;  // Hardcoded font

    // Draw each character
    for (auto ch : text) {
        // Find the glyph descriptor index for the character which is the index
        // of the character in the unicode list
        const auto it =
            std::find(font.unicode_list.begin(), font.unicode_list.end(), ch);
        if (it == font.unicode_list.end()) {
            continue;
        }
        const auto index = std::distance(font.unicode_list.begin(), it);

        const auto &dsc = font.glyph_dsc[index];

        // Get the glyph image and size
        auto glyph = &font.glyph_bitmap[dsc.glyph_index];
        auto byteSize = (dsc.w_px + elementSize - 1) / elementSize;
        auto size = static_cast<size_t>(byteSize * font.h_px);

        // Draw the glyph image
        const std::span glyphSpan{glyph, size};
        Image img = Image(glyphSpan, dsc.w_px);
        drawImage(coord, img);

        // Move to the next character
        coord.x += dsc.w_px;
    }
}
