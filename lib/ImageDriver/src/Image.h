#ifndef IMAGE_H
#define IMAGE_H
#include "./Vec2.h"

/**
 * A tightly packed 1-bit image.
 */
class Image {
public:
    using Element = unsigned;
    constexpr Image(const std::span<const Element> data, size_t width)
        : mData(data), mWidth(width), mHeight(data.size() / width) {
    }

    [[nodiscard]] constexpr size_t width() const {
        return mWidth / (sizeof(Element) * CHAR_BIT);
    }
    [[nodiscard]] constexpr size_t height() const {
        return mHeight / (sizeof(Element) * CHAR_BIT);
    }

    [[nodiscard]] constexpr size_t internalWidth() const {
        return mWidth;
    }

    [[nodiscard]] constexpr size_t internalHeight() const {
        return mHeight;
    }

    [[nodiscard]] constexpr size_t internalSize() const {
        return mData.size();
    }

    [[nodiscard]] constexpr size_t size() const {
        return width() * height();
    }

    [[nodiscard]] static constexpr size_t elementSize() {
        return sizeof(Element) * CHAR_BIT;
    }

    [[nodiscard]] constexpr std::span<const Element> data() const {
        return mData;
    }

    [[nodiscard]] constexpr const Element& operator[](size_t index) const {
        return mData[index];
    }

    [[nodiscard]] constexpr size_t index(Vec2u byte_coord) const {
        if (byte_coord.x >= mWidth || byte_coord.y >= mHeight) {
            return size();
        }
        return byte_coord.y * mWidth + byte_coord.x;
    }

private:
    std::span<const Element> mData;
    size_t mWidth;
    size_t mHeight;
};

#endif //IMAGE_H
