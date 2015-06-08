#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <pangolin/pangolin.h>
#include <string>
#include <vector_types.h>
#include "fonts/font_manager.h"

enum ToolboxSection {
    LabelSection = 0,
    ButtonSection = 1,

    NumSections
};

enum ToolboxButton {
    PointSelectionButton = 0,
    LassoSelectionButton,

    NumButtons
};

class Toolbox {
public:

    Toolbox(const uchar3 * classColors,
            const std::string * classNames,
            const int nClasses,
            FontManager & fontManager);

    ~Toolbox();

    void render(const float2 windowSize);

    inline void setButtonActive(ToolboxButton button, bool active) { assert(button != NumButtons); buttonActive_[button] = active; }

    inline int getButtonSize() { return iconRenderSize_; }

    inline int getButtonSpacing() { return iconRenderSpacing_; }

    ToolboxSection getSection(const float2 point);

    ToolboxButton getButton(const float2 point);

private:

    std::string getIconFilename(ToolboxButton button);

    std::vector<pangolin::GlTexture*> buttonIcons_;
    std::vector<bool> buttonActive_;

    std::vector<int> sectionHeights_;
    std::vector<int> sectionStarts_;

    const int nClasses_;
    const std::string * classNames_;
    const uchar3 * classColors_;

    FontManager & fontManager_;

    static const int labelRowHeight_ = 20;
    static const int labelPadHorizontal_ = 16;
    static const int labelPadVertical_ = 8;
    static const int labelPatchHeight_ = 16;
    static const int labelPatchWidth_ = 32;
    static const int labelFontSize_ = 16;

    static const int iconRenderSize_ = 32;
    static const int iconRenderSpacing_ = 4;

    static const int iconImageSize_ = 64;
    static const int inactiveAlpha_ = 96;

};

#endif // TOOLBOX_H
