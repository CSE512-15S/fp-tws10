#include "font_manager.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftglyph.h>

#include <fontconfig.h>

FontManager::FontManager(const std::string fontName) {

    std::string regularFontFile = getFontFile(fontName,FontStyleRegular);
    std::string boldFontFile = getFontFile(fontName,FontStyleBold);
    std::string italicFontFile = getFontFile(fontName,FontStyleItalic);

    FT_Library ftLibrary;
    if (FT_Init_FreeType(&ftLibrary)) {
        std::cerr << "could not initialize freetype library" << std::endl;
        return;
    }

    // TODO
    const int fontSize = 42;

    regular_ = new FontFace(ftLibrary,regularFontFile,fontSize);
    bold_ = new FontFace(ftLibrary,boldFontFile,fontSize);
    italic_ = new FontFace(ftLibrary,italicFontFile,fontSize);

    FT_Done_FreeType(ftLibrary);

}

FontManager::~FontManager() {
    delete regular_;
    delete bold_;
    delete italic_;
}

void FontManager::printString(const std::string & text, const float x, const float y) {
    regular_->printString(text,x,y);
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
