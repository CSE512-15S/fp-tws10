#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <string>
#include "font_face.h"

class FontManager {
public:
    FontManager(const std::string regularFile,
                const std::string boldFile,
                const std::string italicFile);
    ~FontManager();

    void printString(const std::string & text, const float x, const float y);
private:
    FontFace * regular_;
    FontFace * bold_;
    FontFace * italic_;
};

#endif // FONT_MANAGER_H
