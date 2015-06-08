#include "toolbox.h"

#include <iostream>
#include "util/gl_helpers.h"

Toolbox::Toolbox() : buttonActive_(NumButtons,false) {

    for (int i=0; i<NumButtons; ++i) {
        pangolin::GlTexture * icon = new pangolin::GlTexture(iconImageSize_,iconImageSize_);
        icon->LoadFromFile(getIconFilename((ToolboxButton)i));
        buttonIcons_.push_back(icon);
    }

}

Toolbox::~Toolbox() {

    for (pangolin::GlTexture * texture : buttonIcons_) {
        delete texture;
    }

}

void Toolbox::render(const float2 windowSize) {

    glEnable(GL_BLEND);
    for (int i=0; i<NumButtons; ++i) {
        glColor4ub(255,255,255,buttonActive_[i] ? 255 : inactiveAlpha_);
        const float2 iconLocation = make_float2(i*iconRenderSize_ + (i+1) * iconRenderSpacing_,
                                                windowSize.y - iconRenderSize_ - iconRenderSpacing_);
        renderTexture(*buttonIcons_[i],iconLocation,make_float2(iconRenderSize_));
    }
    glDisable(GL_BLEND);

}

std::string Toolbox::getIconFilename(ToolboxButton button) {
    switch (button) {
        case PointSelectionButton:
            return "../src/icons/pointSelection.png";
            break;
        case LassoSelectionButton:
            return "../src/icons/lassoSelection.png";
            break;
        default:
            std::cerr << "no icon image specified for button " << button << std::endl;
            return "";
    }
}


