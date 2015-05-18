#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <map>
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

    void printString(const std::string & text, const float x, const float y, const int size, const FontStyle style = FontStyleRegular);
    int getStringLength(const std::string & text, const int size, const FontStyle style = FontStyleRegular);
private:
    std::string getFontFile(const std::string fontName, const FontStyle style);
    void loadFontSize(const int size);

    std::string regularFontFile_;
    std::string boldFontFile_;
    std::string italicFontFile_;
    std::map<int,FontFace *> regular_;
    std::map<int,FontFace *> bold_;
    std::map<int,FontFace *> italic_;
};

#endif // FONT_MANAGER_H
