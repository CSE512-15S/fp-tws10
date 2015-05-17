#include "font_manager.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftglyph.h>

FontManager::FontManager(const std::string regularFile,
                         const std::string boldFile,
                         const std::string italicFile) {

    FT_Library ftLibrary;
    if (FT_Init_FreeType(&ftLibrary)) {
        std::cerr << "could not initialize freetype library" << std::endl;
        return;
    }

    // TODO
    const int fontSize = 42;

    regular_ = new FontFace(ftLibrary,regularFile,fontSize);
    bold_ = new FontFace(ftLibrary,boldFile,fontSize);
    italic_ = new FontFace(ftLibrary,italicFile,fontSize);

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
