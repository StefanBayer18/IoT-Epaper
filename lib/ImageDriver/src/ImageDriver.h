#ifndef IMAGEDRIVER_H

#define IMAGEDRIVER_H

#include <climits>
#include <span>
#include <vector>
#include <string_view>
#include "./Image.h"
#include "./Vec2.h"
#include <cstdio>

class ImageDriver {
public:
    /// Internal element type of the image
    using Element = uint8_t;

    ImageDriver(size_t width, size_t height);

    /**
     * Draws a text centered around coordinate.
     *
     * @param coord The coordinates of the middle of the text.
     * @param text The text to draw.
     */
    void drawCenteredText(Vec2u coord, std::string_view text);

    /**
     * Draws a text.
     *
     * @param coord The coordinates of the top left corner of the text.
     * @param text The text to draw.
     */
    void drawText(Vec2u coord, std::string_view text);

    /**
     * Draws an image.
     *
     * @param coord The coordinates of the top left corner of the image.
     * @param image The image to draw.
     */
    void drawImage(Vec2u coord, const Image& image);

    /**
     * Draws a line in the image.
     *
     * @param start Start coordinates of the line
     * @param end End coordinates of the line
     */
    void drawLine(Vec2u start, Vec2u end);

    /**
     * Draws a pixel.
     *
     * @param coord The coordinates of the pixel
     */
    void drawPoint(Vec2u coord);

    /**
     * Draws a rectangle.
     *
     * @param pos Pixel Position in Image.
     * @param size Size of the square.
     */
    void drawFilledRect(Vec2u pos, Vec2u size);
    

    void drawGraph(Vec2u min, Vec2u end, std::span<const Vec2f> points);

    [[nodiscard]] constexpr size_t width() const {
        return mWidth;
    }

    [[nodiscard]] constexpr size_t height() const {
        return mHeight;
    }

    [[nodiscard]] constexpr size_t size() const {
        return mImgSize;
    }

    [[nodiscard]] constexpr uint8_t operator [](size_t index) const {
        return mImg[index];
    }

    constexpr static uint8_t elementSize = sizeof(Element) * CHAR_BIT;

private:
    /**
     * @brief Converts a pixel coordinate to a byte index and a bit mask
     * @param coord The coordinates of the pixel
     * @return A pair consisting of the byte index and a bit mask
     */
    [[nodiscard]] constexpr std::pair<size_t, Element> coord2index(
        Vec2u coord) const {
        if (coord.x >= mWidth || coord.y >= mHeight) {
            return {};
        }
        const size_t index = (coord.y * mInternalWidth) + (coord.x / elementSize);
        const Element mask = static_cast<Element>(1) << (elementSize - (coord.x % elementSize));
        return {index, mask};
    }

    void drawVerticalLine(Vec2u, Element, size_t);

    size_t mWidth;
    size_t mInternalWidth;
    size_t mHeight;
    size_t mImgSize;
    std::vector<Element> mImg;
};

#endif
