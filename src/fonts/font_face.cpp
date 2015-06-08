#include "font_face.h"

#include <iostream>
#include "util/misc.h"

FontFace::FontFace(FT_Library & ftLibrary, const std::string fontFile, const int fontSize) {

    FT_Face ftFace;
    if (FT_New_Face(ftLibrary, fontFile.c_str(), 0, &ftFace)) {
        std::cerr << "could not open font\n" << std::endl;
        return;
    }

    FT_Set_Char_Size( ftFace, fontSize << 6, fontSize << 6, 96, 96);

    glGenTextures(128,textures_);

    for (unsigned char ch=0; ch<128; ++ch) {

        if (FT_Load_Glyph( ftFace, FT_Get_Char_Index( ftFace, ch), FT_LOAD_DEFAULT )) {
            std::cerr << "could not load character " << ch << std::endl;
            return;
        }

        FT_Glyph glyph;
        if (FT_Get_Glyph( ftFace->glyph, &glyph)) {
            std::cerr << "could not get glyph for " << ch << std::endl;
            return;
        }

        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1);
        FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

        FT_Bitmap & bitmap = bitmapGlyph->bitmap;

        int width = nextP2( bitmap.width );
        int height = nextP2( bitmap.rows );

        GLubyte * expandedData = new GLubyte[2*width*height];
        for(int j=0; j <height;j++) {
            for(int i=0; i < width; i++){
                expandedData[2*(i+j*width)]= expandedData[2*(i+j*width)+1] =
                    (i>=bitmap.width || j>=bitmap.rows) ?
                    0 : bitmap.buffer[i + bitmap.width*j];
            }
        }

        glBindTexture( GL_TEXTURE_2D, textures_[ch] );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData );

        delete [] expandedData;

        float x = (float)bitmap.width / (float)width,
              y = (float)bitmap.rows / (float)height;

        glyphInfo_[ch].left = bitmapGlyph->left;
        glyphInfo_[ch].top = bitmapGlyph->top;
        glyphInfo_[ch].rows = bitmap.rows;
        glyphInfo_[ch].width = bitmap.width;
        glyphInfo_[ch].advance = ftFace->glyph->advance.x >> 6;
        glyphInfo_[ch].texWidth = width;
        glyphInfo_[ch].texHeight = height;
    }

    FT_Done_Face(ftFace);
}

int FontFace::getStringLength(const std::string &text) {
    int length = 0;
    for (int i=0; i<text.size(); ++i) {
        length += getLetterLength(text.c_str()[i]);
    }
    return length;
}

int FontFace::getLetterLength(const char letter) {
    return glyphInfo_[letter].left + glyphInfo_[letter].advance;
}

void FontFace::printString(const std::string & text, float x, float y) {

    glPushMatrix();
    glTranslatef(x,y,0);
    for (int i=0; i<text.size(); ++i) {
        printLetter(text.c_str()[i]);
    }
    glPopMatrix();

}

void FontFace::printLetter(const char letter) {
    glBindTexture(GL_TEXTURE_2D,textures_[letter]);
    glTranslatef(glyphInfo_[letter].left,0,0);

    glPushMatrix();
    glTranslatef(0,glyphInfo_[letter].top-glyphInfo_[letter].rows,0);

    float x = (float)glyphInfo_[letter].width / (float)glyphInfo_[letter].texWidth,
          y = (float)glyphInfo_[letter].rows / (float)glyphInfo_[letter].texHeight;

    glBegin(GL_QUADS);
    glTexCoord2d(0,0);       glVertex2f(0,                 glyphInfo_[letter].rows);
    glTexCoord2d(0,y);       glVertex2f(0,                 0);
    glTexCoord2d(x,y);       glVertex2f(glyphInfo_[letter].width,0);
    glTexCoord2d(x,0);       glVertex2f(glyphInfo_[letter].width,glyphInfo_[letter].rows);
    glEnd();
    glPopMatrix();
    glTranslatef(glyphInfo_[letter].advance,0,0);
}
