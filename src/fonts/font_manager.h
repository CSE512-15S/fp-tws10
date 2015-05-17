#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <string>
#include "font_face.h"

enum FontStyle {
    FontStyleRegular,
    FontStyleBold,
    FontStyleItalic
};

class FontManager {
public:
    FontManager(const std::string fontName);
    ~FontManager();

    void printString(const std::string & text, const float x, const float y);
private:
    std::string getFontFile(const std::string fontName, const FontStyle style);

    FontFace * regular_;
    FontFace * bold_;
    FontFace * italic_;
};

#endif // FONT_MANAGER_H
