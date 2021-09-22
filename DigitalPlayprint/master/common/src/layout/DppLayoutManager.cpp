// COPYRIGHT_BEGIN
// COPYRIGHT_END

#include "DppLayoutManager.h"

DppLayoutManager *
DppLayoutManager::gDppLayoutManager = nullptr;

LayoutState *
DppLayoutManager::getState() {
    return mState;
}

void
DppLayoutManager::setState(LayoutState *state) {
    mState = state;
}
