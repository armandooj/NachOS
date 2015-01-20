#ifdef CHANGED
#include "frameprovider.h"

FrameProvider::FrameProvider(int numFrames) {
  bitmap = new BitMap(numFrames);
}

FrameProvider::~FrameProvider() {
  delete(bitmap);
}


#endif