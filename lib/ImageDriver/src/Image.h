/**
 * @brief A tightly packed 1-bit image.
 * @author AntonPieper
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <climits>
#include <cstddef>
#include <span>

#include "./Vec2.h"

/**
 * A tightly packed 1-bit image.
 */
class Image {
public:
    /// @brief The storage type for the image data.
    using Element = uint8_t;
    /// @brief The number of bits in an element.
    static constexpr size_t elementSize = sizeof(Element) * CHAR_BIT;

    /**
     * @brief Ceil the value to the next multiple of elementSize.
     *
     * @param value The value to ceil.
     * @return The next multiple of elementSize.
     */
    static constexpr size_t ceil(size_t value) {
        return (value + elementSize - 1) / elementSize;
    }

    constexpr Image(const std::span<const Element> data, size_t width)
        : mData(data), mWidth(width), mHeight(data.size() / ceil(width)) {}

    [[nodiscard]] constexpr size_t width() const { return mWidth; }
    [[nodiscard]] constexpr size_t height() const { return mHeight; }

    [[nodiscard]] constexpr size_t byteWidth() const { return ceil(mWidth); }

    [[nodiscard]] constexpr size_t size() const { return width() * height(); }
    [[nodiscard]] constexpr size_t byteSize() const {
        return byteWidth() * height();
    }

    [[nodiscard]] constexpr std::span<const Element> data() const {
        return mData;
    }

    [[nodiscard]] constexpr const Element& operator[](size_t index) const {
        return mData[index];
    }

    /**
     * @brief Get the byte index of a byte in the image.
     *
     * @param byte_coord The byte coordinate.
     * @return Returns the byte index or max size_t if the coordinate is out of
     * bounds.
     */
    [[nodiscard]] constexpr size_t index(Vec2u byte_coord) const {
        if (byte_coord.x >= mWidth || byte_coord.y >= mHeight) {
            return std::numeric_limits<size_t>::max();
        }
        return byte_coord.y * mWidth + byte_coord.x;
    }

private:
    std::span<const Element> mData;
    size_t mWidth;
    size_t mHeight;
};

#endif  // IMAGE_H
