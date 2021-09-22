// COPYRIGHT_BEGIN
// COPYRIGHT_END

#ifndef __DPPLAYOUTMANAGER_H_
#define __DPPLAYOUTMANAGER_H_

#include "pplayout.h"

class DppLayoutManager
{
  private:
    // The Singleton instance of the DPP Layout Manager.
    static DppLayoutManager *gDppLayoutManager;

  public:
    /**
     * @brief Retrieve the DPP Layout Manager.
     *
     * @return A pointer to the Singleton instance of the DPP Layout
     * Manager is returned.
     */
    static DppLayoutManager *getInstance() {
        if (gDppLayoutManager == nullptr)
            gDppLayoutManager = new DppLayoutManager();
        return gDppLayoutManager;
    }

    /**
     * @brief Get the DPP Layout Manager state.
     *
     * @return A reference to the DPP Layout Manager state is
     * returned.
     */
    LayoutState *getState();
    void setState(LayoutState *state);

  private:
    // Hide default constructor.
    DppLayoutManager() {};

    // The layout state used by the DPP Python commands to construct
    // a Digital Playprint.
    LayoutState *mState;
};

#endif // __DPPLAYOUTMANAGER_H_
