#include "./ImageDriver.h"

#include <algorithm>
#include <bitset>
#include <string_view>

#include "./Font.h"
#include "./JetBrains.h"
#include "esp_log.h"

#define IMAGETAG "IMAGEDRIVER"

ImageDriver::ImageDriver(size_t width, size_t height)
    : mWidth(width),
      mInternalWidth(width / elementSize),
      mHeight(height),
      mImgSize(mInternalWidth * height),
      mImg(mImgSize, 0) {}

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
    // Horizontal Line
    if(from.x == to.x){
        const auto [index, mask] = coord2index({from.x, from.y});
        for(int y = 0; y < to.y - from.y; y++){
            mImg[index + y * mInternalWidth] |= mask;
        }
    }
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
        mImg[index] |= mask;
    }
}

void ImageDriver::drawCenteredText(Vec2u coord, std::string_view text){
    const Font &font = JetBrains::font24;
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
    drawText({coord.x - width/2, coord.y}, text);
}

void ImageDriver::drawText(Vec2u coord, std::string_view text) {
    const Font &font = JetBrains::font24;
    for (auto ch : text) {
        const auto it =
            std::find(font.unicode_list.begin(), font.unicode_list.end(), ch);
        if (it == font.unicode_list.end()) {
            // std::cerr << "Character" <<  ch << "not found in font" <<
            // std::endl;
            //ESP_LOGI(IMAGETAG, "Char not found\n");
            continue;
        }
        const auto index = std::distance(font.unicode_list.begin(), it);
        const auto &dsc = font.glyph_dsc[index];

        // TODO: See if this actually works
        auto glyph = &font.glyph_bitmap[dsc.glyph_index];
        auto byteSize = (dsc.w_px + elementSize - 1) / elementSize;
        auto size = static_cast<size_t>(byteSize * font.h_px);
        const std::span glyphSpan{glyph, size};
        Image img = Image(glyphSpan, dsc.w_px);
        drawImage(coord, img);
        coord.x += dsc.w_px;
    }
}
