#include "selection_manager.h"

constexpr float SelectionManager::noneSelectedVal_;

SelectionManager::SelectionManager(const int nItems) :
    selection_(nItems,noneSelectedVal_) { }

void SelectionManager::setSelection(const int item) {
    assert(item >= 0 && item < selection_.size());
    setAll(unselectedVal_);
    selection_[item] = selectedVal_;
    hasSelection_ = true;
}

void SelectionManager::setSelection(const std::vector<int> & items) {
    setAll(unselectedVal_);
    for (int i=0; i<items.size(); ++i) {
        selection_[items[i]] = selectedVal_;
    }
    hasSelection_ = true;
}

void SelectionManager::appendSelection(const int item) {
    assert(item >= 0 && item < selection_.size());
    if (!hasSelection_) {
        setAll(unselectedVal_);
        hasSelection_ = true;
    }
    selection_[item] = selectedVal_;
}
