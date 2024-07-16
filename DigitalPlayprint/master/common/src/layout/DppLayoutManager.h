// COPYRIGHT_BEGIN
//
// Copyright (c) 2024 Wizzer Works
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//  For information concerning this source code, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
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
