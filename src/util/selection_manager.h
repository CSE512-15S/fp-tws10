#ifndef SELECTION_MANAGER_H
#define SELECTION_MANAGER_H

#include <cstring>
#include <vector>
#include <assert.h>

class SelectionManager {
public:

    SelectionManager(const int nItems);

    inline bool hasSelection() { return hasSelection_; }

    inline void clearSelection() { setAll(noneSelectedVal_); hasSelection_ = false; }

    void setSelection(const int item);

    void setSelection(const std::vector<int> & items);

    void appendSelection(const int item);

    std::vector<float> & getSelection() { return selection_; }

    static constexpr float selectedVal_ = 1.f;
    static constexpr float unselectedVal_ = 0.f;
    static constexpr float noneSelectedVal_ = 0.5f;
private:

    inline void setAll(float val) { std::fill(selection_.begin(),selection_.end(), val); }

    bool hasSelection_;
    std::vector<float> selection_;



};

#endif // SELECTION_MANAGER_H
