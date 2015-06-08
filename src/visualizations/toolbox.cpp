#include "toolbox.h"

#include <iostream>
#include "util/gl_helpers.h"

Toolbox::Toolbox(const uchar3 * classColors,
                 const std::string * classNames,
                 const int nClasses,
                 FontManager & fontManager) :
    buttonActive_(NumButtons,false),
    nClasses_(nClasses),
    classColors_(classColors),
    classNames_(classNames),
    fontManager_(fontManager),
    sectionHeights_(NumSections),
    sectionStarts_(NumSections+1) {

    for (int i=0; i<NumButtons; ++i) {
        pangolin::GlTexture * icon = new pangolin::GlTexture(iconImageSize_,iconImageSize_);
        icon->LoadFromFile(getIconFilename((ToolboxButton)i));
        buttonIcons_.push_back(icon);
    }

    sectionHeights_[LabelSection] = nClasses_*labelRowHeight_ + 2*labelPadVertical_;
    sectionHeights_[ButtonSection] = iconRenderSize_ + 2*iconRenderSpacing_;

    int runningTotal = 0;
    for (int i=0; i<NumSections; ++i) {
        sectionStarts_[i] = runningTotal;
        runningTotal += sectionHeights_[i];
    }
    sectionStarts_[NumSections] = runningTotal;

}

Toolbox::~Toolbox() {

    for (pangolin::GlTexture * texture : buttonIcons_) {
        delete texture;
    }

}

void Toolbox::render(const float2 windowSize) {

    // -=-=-=- label section -=-=-=-
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glColor3ub(0,0,0);
    uchar3 colors[nClasses_*4];
    float vertices[nClasses_*4*2];
    for (int i=0; i<nClasses_; ++i) {
        colors[4*i] = colors[4*i+1] = colors[4*i+2] = colors[4*i+3] = classColors_[i];
        vertices[8*i  ] = labelPadHorizontal_;                    vertices[8*i+1] = windowSize.y - labelPadVertical_ - i*labelRowHeight_ - (labelRowHeight_-labelPatchHeight_)/2;
        vertices[8*i+2] = labelPadHorizontal_ + labelPatchWidth_; vertices[8*i+3] = windowSize.y - labelPadVertical_ - i*labelRowHeight_ - (labelRowHeight_-labelPatchHeight_)/2;
        vertices[8*i+4] = labelPadHorizontal_ + labelPatchWidth_; vertices[8*i+5] = windowSize.y - labelPadVertical_ - (i+1)*labelRowHeight_ + (labelRowHeight_-labelPatchHeight_)/2;
        vertices[8*i+6] = labelPadHorizontal_;                    vertices[8*i+7] = windowSize.y - labelPadVertical_ - (i+1)*labelRowHeight_ + (labelRowHeight_-labelPatchHeight_)/2;
        fontManager_.printString(classNames_[i],
                                 labelPadHorizontal_*2 + labelPatchWidth_,
                                 windowSize.y - (labelPadVertical_ +(i+1)*labelRowHeight_ - (labelRowHeight_ - labelFontSize_)/2),
                                 labelFontSize_);
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,vertices);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
    glDrawArrays(GL_QUADS,0,nClasses_*4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // -=-=-=- button section
    const int buttonSectionStart = sectionStarts_[ButtonSection];
    glEnable(GL_BLEND);
    for (int i=0; i<NumButtons; ++i) {
        glColor4ub(255,255,255,buttonActive_[i] ? 255 : inactiveAlpha_);
        const float2 iconLocation = make_float2(i*iconRenderSize_ + (i+1) * iconRenderSpacing_,
                                                windowSize.y - buttonSectionStart -iconRenderSize_ - iconRenderSpacing_);
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

ToolboxSection Toolbox::getSection(const float2 point) {

    for (int i = 0; i < NumSections; ++i) {
        if (point.y < sectionStarts_[i+1]) {
            return (ToolboxSection)i;
        }
    }
    return NumSections;

}

ToolboxButton Toolbox::getButton(const float2 point) {

    const float2 buttonPoint = point - make_float2(0,sectionStarts_[ButtonSection]);
    const int buttonCol = buttonPoint.x / (iconRenderSize_ + iconRenderSpacing_);
    if (buttonCol < NumButtons) {
        return (ToolboxButton)buttonCol;
    } else {
        return NumButtons;
    }

}
