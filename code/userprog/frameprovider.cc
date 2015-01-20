#ifdef CHANGED
#include "frameprovider.h"

FrameProvider::FrameProvider(int numFrames) {
  framesBitMap = new BitMap(numFrames);
  framesBitMap->Mark(0);
}

FrameProvider::~FrameProvider() {
  delete(framesBitMap);
}

int FrameProvider::GetEmptyFrame() {
  // TODO Random()
  return framesBitMap->Find();
}

void FrameProvider::ReleaseFrame(int frame) {
  framesBitMap->Clear(frame);
}

int FrameProvider::NumAvailFrame() {
  return framesBitMap->NumClear();
}


#endif