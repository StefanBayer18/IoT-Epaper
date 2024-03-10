/**
 * @brief A 2D vector.
 * @author AntonPieper
 */

#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <cstddef>

/**
 * @brief A 2D vector.
 *
 * @tparam T The type of the vector components.
 */
template <typename T>
struct Vec2 {
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}
    constexpr Vec2(Vec2 const&) = default;
    constexpr Vec2(Vec2&&) = default;

    template <typename U>
    constexpr explicit operator Vec2<U>() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    constexpr Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(const T& other) {
        x *= other;
        y *= other;
        return *this;
    }

    constexpr Vec2& operator/=(const T& other) {
        x /= other;
        y /= other;
        return *this;
    }

    constexpr Vec2 operator+(const Vec2& other) const {
        return Vec2(*this) += other;
    }

    constexpr Vec2 operator-(const Vec2& other) const {
        return Vec2(*this) -= other;
    }

    constexpr Vec2 operator*(const T& other) const {
        return Vec2(*this) *= other;
    }

    constexpr Vec2 operator/(const T& other) const {
        return Vec2(*this) /= other;
    }

    constexpr T& operator[](size_t index) { return index == 0 ? x : y; }

    constexpr const T& operator[](size_t index) const {
        return index == 0 ? x : y;
    }

    [[nodiscard]] constexpr T length() const { return std::hypot(x, y); }
    T x;
    T y;
};

using Vec2u = Vec2<size_t>;
using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

#endif  // VEC2_H
