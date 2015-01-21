#ifdef CHANGED

/*
* Management of frames.
* This class encapsulates the allocation of physical pages to virtual pages.
*/

#include "bitmap.h"
#include "synch.h"

class FrameProvider {

  public:
    FrameProvider(int numFrames);
    ~FrameProvider();

    int GetEmptyFrame();
    void ReleaseFrame(int frame);
    int NumAvailFrame();

  private:
    BitMap *framesBitMap;
    Lock *lock;  
};

#endif