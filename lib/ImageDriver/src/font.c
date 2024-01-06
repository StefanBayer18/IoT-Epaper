#ifndef JOYSTIX_FONT
#define JOYSTIX_FONT

#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static const uint8_t glyphBitmap[] = {
    /* U+0025 "%" */
    0xe0, 0xf8, 0x40, 0x0, 0x18, 0x18, 0x0, 0x2,
    0x1c, 0x7, 0xc1, 0xc0,

    /* U+0030 "0" */
    0xf, 0x0, 0x10, 0x31, 0xcf, 0xf, 0xf0, 0xff,
    0xf, 0xf0, 0xf3, 0x8c, 0x38, 0xc3, 0x8c, 0xf,
    0x0,

    /* U+0031 "1" */
    0x1c, 0x7, 0x7, 0xc0, 0x70, 0x1c, 0x7, 0x1,
    0xc0, 0x70, 0x1c, 0x7, 0xf, 0xfc,

    /* U+0032 "2" */
    0x3f, 0x84, 0x13, 0x83, 0x81, 0xf0, 0x3e, 0x3f,
    0xf, 0xc1, 0xf8, 0xf8, 0x1f, 0x3, 0xff, 0x80,

    /* U+0033 "3" */
    0x3f, 0xe0, 0x38, 0x7, 0x3, 0x80, 0x70, 0x1f,
    0x80, 0x38, 0x7, 0xe0, 0xe4, 0x18, 0xff, 0x0,

    /* U+0034 "4" */
    0x7, 0x80, 0xf0, 0x7e, 0x1d, 0xc3, 0xb9, 0xc7,
    0x38, 0xe7, 0xff, 0x3, 0x80, 0x70, 0xe, 0x0,

    /* U+0035 "5" */
    0xff, 0xdc, 0x3, 0x80, 0x7f, 0xe0, 0xc, 0x1,
    0xc0, 0x38, 0x7, 0xe0, 0xe4, 0x18, 0xff, 0x0,

    /* U+0036 "6" */
    0x1f, 0x87, 0x0, 0xe0, 0x70, 0xe, 0x1, 0xff,
    0x38, 0x3f, 0x7, 0xe0, 0xe4, 0x10, 0xfe, 0x0,

    /* U+0037 "7" */
    0xff, 0xfc, 0x1f, 0x83, 0x81, 0xc0, 0x78, 0xe,
    0x1, 0x0, 0xe0, 0x1c, 0x3, 0x80, 0x70, 0x0,

    /* U+0038 "8" */
    0x3f, 0x4, 0x23, 0x83, 0x7c, 0x6f, 0x8c, 0x7e,
    0x31, 0xfe, 0x3f, 0xc0, 0xe8, 0x18, 0xff, 0x0,

    /* U+0039 "9" */
    0x3f, 0x84, 0x13, 0x83, 0xf0, 0x7e, 0xe, 0x7f,
    0xc0, 0x38, 0x7, 0x3, 0x80, 0x70, 0xfc, 0x0,

    /* U+00B0 "°" */
    0x7c, 0xe7, 0xe7, 0xe7, 0x64, 0x7c
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/
typedef struct GlyphDescription {
    size_t bitmap_index;
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t ofs_x;
    int8_t ofs_y;
} GlyphDescription;
static const GlyphDescription glyphDsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0,
     .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 212, .box_w = 10, .box_h = 9, .ofs_x = 2,
     .ofs_y = 2},
    {.bitmap_index = 12, .adv_w = 212, .box_w = 12, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 212, .box_w = 10, .box_h = 11, .ofs_x = 2,
     .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 91, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 212, .box_w = 11, .box_h = 11, .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 212, .box_w = 8, .box_h = 6, .ofs_x = 3,
     .ofs_y = 7}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicodeList0[] = {
    0x0, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x8b
};

typedef struct Font {
    const uint8_t lineHeight;
    const uint8_t* glyphBitmap;
    const uint16_t* characterMapping;
    const GlyphDescription* glyphDsc;
} Font;
/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
const Font font = {
    .lineHeight = 13, /*The maximum line height required by the font*/
    .glyphBitmap = glyphBitmap, /*Bitmap of glyphs*/
    .characterMapping = unicodeList0, /*Index of each character*/
    .glyphDsc = glyphDsc, /*Description of each glyph*/
};
#endif
