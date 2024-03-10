/**
 * @author AntonPieper
 * @brief Font data structure
 */

#ifndef FONT_H
#define FONT_H

#include <cstdint>
#include <span>

/**
 * @brief Structure containing information about a glyph
 *
 */
struct GlyphDescription {
    /** Glyph width in pixels */
    uint8_t w_px;
    /** Index of the glyph in the bitmap */
    uint16_t glyph_index;
};

/**
 * @brief Structure containing information about a font
 *
 */
struct Font {
    /** List of unicode characters */
    std::span<const uint8_t> unicode_list;
    /** Description of glyphs */
    std::span<const GlyphDescription> glyph_dsc;
    /** Bitmap of glyphs */
    std::span<const uint8_t> glyph_bitmap;
    /** Number of glyphs in the font */
    uint16_t glyph_cnt;
    /** Last Unicode letter in this font */
    uint16_t unicode_last;
    /** First Unicode letter in this font */
    uint16_t unicode_first;
    /** Font height in pixels */
    uint8_t h_px;
};

#endif  // FONT_H
