#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <pangolin/pangolin.h>
#include <string>
#include <vector_types.h>

enum ToolboxSection {
    ButtonSection = 0,

    NumSections
};

enum ToolboxButton {
    PointSelectionButton = 0,
    LassoSelectionButton,

    NumButtons
};

class Toolbox {
public:

    Toolbox();

    ~Toolbox();

    void render(const float2 windowSize);

    inline void setButtonActive(ToolboxButton button, bool active) { assert(button != NumButtons); buttonActive_[button] = active; }

    inline int getButtonSize() { return iconRenderSize_; }

    inline int getButtonSpacing() { return iconRenderSpacing_; }

private:

    std::string getIconFilename(ToolboxButton button);

    std::vector<pangolin::GlTexture*> buttonIcons_;
    std::vector<bool> buttonActive_;

    static const int iconRenderSize_ = 32;
    static const int iconRenderSpacing_ = 4;

    static const int iconImageSize_ = 64;
    static const int inactiveAlpha_ = 96;

};

#endif // TOOLBOX_H
