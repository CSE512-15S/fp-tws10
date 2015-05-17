#ifndef FONT_FACE_H
#define FONT_FACE_H

#include <string>
#include <GL/gl.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftglyph.h>

class FontFace {
public:
    FontFace(FT_Library & ftLibrary, const std::string fontFile, const int fontSize);

    void printString(const std::string & text, float x, float y);

private:
    void printLetter(const char letter);

    struct GlyphInfo {
        int left, top, width, rows, advance, texWidth, texHeight;
    };

    GlyphInfo glyphInfo_[128];
    GLuint textures_[128];
};

#endif // FONT_FACE_H
