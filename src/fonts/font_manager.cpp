#include "font_manager.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftglyph.h>

#include <fontconfig.h>

FontManager::FontManager(const std::string fontName) {

    regularFontFile_ = getFontFile(fontName,FontStyleRegular);
    boldFontFile_ = getFontFile(fontName,FontStyleBold);
    italicFontFile_ = getFontFile(fontName,FontStyleItalic);
}

FontManager::~FontManager() {
    for (std::pair<int,FontFace *> ff : regular_) { delete ff.second; }
    for (std::pair<int,FontFace *> ff : bold_) { delete ff.second; }
    for (std::pair<int,FontFace *> ff : italic_) { delete ff.second; }
}

void FontManager::printString(const std::string & text, const float x, const float y, const int size, const FontStyle style) {
    if (regular_.find(size) == regular_.end()) {
        loadFontSize(size);
    }
    switch (style) {
    case FontStyleRegular:
        regular_[size]->printString(text,x,y);
        break;
    case FontStyleBold:
        bold_[size]->printString(text,x,y);
        break;
    case FontStyleItalic:
        italic_[size]->printString(text,x,y);
        break;
    }

}

int FontManager::getStringLength(const std::string & text, const int size, const FontStyle style) {
    if (regular_.find(size) == regular_.end()) {
        loadFontSize(size);
    }
    switch (style) {
    case FontStyleRegular:
        return regular_[size]->getStringLength(text);
    case FontStyleBold:
        return bold_[size]->getStringLength(text);
    case FontStyleItalic:
        return italic_[size]->getStringLength(text);
    }

}

std::string FontManager::getFontFile(const std::string fontName, const FontStyle style) {

    FcBool result = FcInit();
    if (!result) {
        std::cerr << "could not initialize fontconfig" << std::endl;
        return "";
    }

    FcConfig * config = FcInitLoadConfigAndFonts();
    std::string name;
    switch (style) {
    case FontStyleRegular:
        name = fontName;
        break;
    case FontStyleBold:
        name = fontName + ":weight=bold";
        break;
    case FontStyleItalic:
        name = fontName + ":italic";
        break;
    }
    FcPattern * pattern = FcNameParse((const FcChar8 *)name.c_str()); FcPatternCreate();
    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    std::string fontFile;
    FcResult res;
    FcPattern * font = FcFontMatch(config, pattern, &res);
    if (font)
    {
       FcChar8 * file = NULL;
       if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
       {
          fontFile = (char*)file;
       }
       FcPatternDestroy(font);
    }

    FcPatternDestroy(pattern);
    FcConfigDestroy(config);

    return fontFile;
}

void FontManager::loadFontSize(const int size) {
    FT_Library ftLibrary;
    if (FT_Init_FreeType(&ftLibrary)) {
        std::cerr << "could not initialize freetype library" << std::endl;
        return;
    }

    regular_[size] = new FontFace(ftLibrary,regularFontFile_,size);
    bold_[size] = new FontFace(ftLibrary,boldFontFile_,size);
    italic_[size] = new FontFace(ftLibrary,italicFontFile_,size);

    FT_Done_FreeType(ftLibrary);
}
