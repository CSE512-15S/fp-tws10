#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <pangolin/pangolin.h>
#include <string>
#include <vector_types.h>
#include "fonts/font_manager.h"
#include "embedding_viz.h"

enum ToolboxSection {
    LabelSection = 0,
    ButtonSection,
    OverviewSection,

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
            FontManager & fontManager,
            const int overviewWidth,
            const int overviewHeight,
            pangolin::GlTexture & overviewTex);

    ~Toolbox();

    void render(const float2 windowSize);

    inline void setButtonActive(ToolboxButton button, bool active) { assert(button != NumButtons); buttonActive_[button] = active; }

    inline void setOverviewImage(uchar3 * overviewImage) { overviewTex_.Upload(overviewImage,GL_RGB,GL_UNSIGNED_BYTE); }

    inline int getButtonSize() { return iconRenderSize_; }

    inline int getButtonSpacing() { return iconRenderSpacing_; }

    inline void setActiveEmbeddingViz(EmbeddingViz * viz) { activeEmbeddingViz_ = viz; }

    ToolboxSection getSection(const float2 point);

    ToolboxButton getButton(const float2 point);

    int getClass(const float2 point);

    void processOverviewCentering(const float2 point, const float windowWidth);

    inline void processZoom(const float zoomFactor) {
        activeEmbeddingViz_->setZoom(activeEmbeddingViz_->getZoom()*zoomFactor);
    }

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

    const int overviewWidth_;
    const int overviewHeight_;
    pangolin::GlTexture & overviewTex_;

    EmbeddingViz * activeEmbeddingViz_;

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

    static constexpr float showOverviewZoomThreshold_ = 0.8f;
    static constexpr float switchToCrosshairZoomThreshold_ = 0.05f;
    static const int crosshairSize_ = 10;

};

#endif // TOOLBOX_H
