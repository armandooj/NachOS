#ifdef CHANGED

/*
* Management of frames.
* This class encapsulates the allocation of physical pages to virtual pages.
*/

#include "bitmap.h"

class FrameProvider {

  public:
    FrameProvider(int numFrames);
    ~FrameProvider();

    int GetEmptyFrame();
    void ReleaseFrame(int frame);
    int NumAvailFrame();

  private:
    BitMap *framesBitMap;
  
};
#endif